//  implementação do simulated annealing para acha o minimo global da função f(x, y) = x^2 + y^2 + 5xy - 4

#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <random>

using namespace std;


float Random (float min, float max){
    random_device seed;
    mt19937 gen(seed());
    uniform_real_distribution<float> dis(min, max);
    return dis(seed);
}



class Solution{
    public:
        Solution();
        ~Solution();
        double Initialize();
        double Perturb(double temperature, double initialTemperature);
        double GetError();
        virtual Solution& operator =(const Solution& init) = 0;
        protected:
        double error;
    private:
        virtual void OnInitialize()=0;
        virtual void OnPerturb(double temperature, double initialTemperature)=0;
        virtual double OnComputeError()=0;
};

Solution::Solution(){
    error = 1.0;
}

Solution::~Solution(){
}

double Solution::GetError(){
    return error;
}

double Solution::Initialize(){
    OnInitialize();
    error = fabs(OnComputeError());
    return error;
}

double Solution::Perturb(double temperature, double initialTemperature){
    OnPerturb(temperature, initialTemperature);
    error = fabs(OnComputeError());
    return error;
}



class NumEq : public Solution{
    public:
        NumEq();
        ~NumEq();
        double x, y;
        Solution& operator =(const Solution& init);
    private:
        void OnInitialize();
        void OnPerturb(double temperature, double initialTemperature);
        double OnComputeError();
};

NumEq::NumEq(){
}

NumEq::~NumEq(){
}

Solution& NumEq::operator =(const Solution& init){
    NumEq& eqInit = (NumEq&)init;
    x = eqInit.x;
    y = eqInit.y;
    error = eqInit.error;
    return *this;
}

void NumEq::OnInitialize(){
    x = Random(-10.0, 10.0);
    y = Random(-10.0, 10.0);
}

void NumEq::OnPerturb(double temperature, double initialTemperature){
    const double ratio = temperature/initialTemperature;
    x = (1.0 - ratio)*x + ratio*Random(-10.0, 10.0);
    y = (1.0 - ratio)*y + ratio*Random(-10.0, 10.0);
}

double NumEq::OnComputeError(){
    return x*x+y*y;
}



class SimulatedAnnealing{
    public:
        SimulatedAnnealing();
        ~SimulatedAnnealing();
        int numTemps;
        int numIterations;
        double initialTemp;
        bool isCoolingScheduleLinear;
        int cycles;
        double Start(Solution& solution, Solution& wk1, Solution& wk2, double goal);
    private:
        double GetTemperature(int index);
        bool IsAcceptedByMetropolis(double temperature, double deltaError);
        double Anneal(Solution& solution, Solution& wk1, Solution& wk2, double goal);
        double EstimateK(Solution& solution, int N);
        double finalTemp;
        double k;
};

SimulatedAnnealing::SimulatedAnnealing(){
    numTemps=100;
    numIterations=100;
    initialTemp=100.0;
    finalTemp=0.0001;
    isCoolingScheduleLinear=false;
    k = 10;
    cycles = 4;
}

SimulatedAnnealing::~SimulatedAnnealing(){}

double SimulatedAnnealing::Start(Solution& solution, Solution& wk1, Solution& wk2, double
goal){
    for(int i=0; i<cycles; i++){
        if (Anneal(solution, wk1, wk2, goal)<=goal) 
            break;
    }
    return solution.GetError();
}

double SimulatedAnnealing::EstimateK(Solution& solution, int N)
{
    double E = 1.0;
    double sum = 0.0;
    double sums = 0.0;
    for(int i = 0; i<N; i++){
        E = solution.Perturb(initialTemp, initialTemp);
        sum+=E;
        sums+=(E*E);
    }
    double variance = sums/(N-1) - (sum*sum)/(N*(N-1));
    return -log(0.8)*initialTemp/sqrt(variance);
}

double SimulatedAnnealing::Anneal(Solution& solution, Solution& wk1, Solution& wk2, double
goal){
    double error = solution.Initialize();
    if (error<=goal) return error; //We are alredy done. Unlikely!
    k = EstimateK(solution, 1000);
    wk1 = solution;
    wk2 = solution;

    finalTemp = goal;
    
    bool hasImproved = false;
    double temperature, deltaError;
    int i;

    for (int n=0; n<numTemps; n++){
        temperature = GetTemperature(n);
        hasImproved = false;
        //_________________________________________ Iterate at this temperature
        for (i=0; i<numIterations; i++){
            deltaError = wk1.Perturb(temperature, initialTemp) - error;
            if (IsAcceptedByMetropolis(temperature, deltaError)){
                wk2 = wk1;
                hasImproved = true;
                if (wk1.GetError()<=goal) //talvez seja work1 a variavel ao inves de wk1
                    break;
            }
        }
        if (hasImproved==true){ // If saw improvement at this temperature
            wk1 = wk2;
            solution = wk2;
            error = solution.GetError();
            if (error<=goal) 
                break;
        }
    }
    return solution.GetError();
}

bool SimulatedAnnealing::IsAcceptedByMetropolis(double temperature, double deltaError){
    if (deltaError <= 0) 
        return true;
    return Random(0.0, 1.0) < exp(-k*deltaError/temperature);
}

double SimulatedAnnealing::GetTemperature(int index){
    if (isCoolingScheduleLinear){
        return initialTemp+index*(finalTemp-initialTemp) / (numTemps-1);
    }
    else{
        return initialTemp*exp(index * log(finalTemp/initialTemp) / (numTemps-1));
    }
}





int main(){
    NumEq solution, wk1, wk2;
    SimulatedAnnealing sa;
    sa.initialTemp = 10;
    sa.numTemps = 2500;
    cout <<"\r\nError = "<<sa.Start(solution, wk1, wk2, 0.00001);
    cout <<"\r\nx = "<<solution.x;
    cout<<"\r\ny = "<<solution.y;
    return 0;
}