//
//  Some calculating functions referenced from LFR Benchmark.
//
//  Thanks to Andrea Lancichinetti, Santo Fortunato and Filippo Radicchi
//

#ifndef DynamicGraph_functions_h
#define DynamicGraph_functions_h
#include <deque>
#include <cmath>
#include <cstdlib>
using namespace std;

int randint(int n) {					// returns an integer number from 0 to n
	return (int(rand()*(n+1)));
}

double rand01(){ // return random float number in [0, 1)
    return double(rand()) / RAND_MAX;
}

// it computes the integral of a power law
double integral (double a, double b) {
	if (fabs(a+1.)>1e-10)
		return (1./(a+1.)*pow(b, a+1.));
	else
		return (log(b));
}
// it returns the average degree of a power law
double average_degree(const double &dmax, const double &dmin, const double &gamma) {
	return (1./(integral(gamma, dmax)-integral(gamma, dmin)))*(integral(gamma+1, dmax)-integral(gamma+1, dmin));
}

// this function sets "cumulative" as the cumulative function of (1/x)^tau, with range= [min, n]
int powerlaw (int n, int min, double tau, deque<double> &cumulative) {
	cumulative.clear();
	double a=0;
	for (double h=min; h<n+1; h++)
		a+= pow((1./h),tau);
    
	double pf=0;
	for(double i=min; i<n+1; i++) {
		pf+=1/a*pow((1./(i)),tau);
		cumulative.push_back(pf);
	}
	return 0;
}

//bisection method to find the inferior limit, in order to have the expected average degree
double solve_dmin(const double& dmax, const double &dmed, const double &gamma) {
	
	double dmin_l=1;
	double dmin_r=dmax;
	double average_k1=average_degree(dmin_r, dmin_l, gamma);
	double average_k2=dmin_r;
	
	if ((average_k1-dmed>0) || (average_k2-dmed<0)) {
		cerr<<"ERROR: the average degree is out of range:";
		if (average_k1-dmed>0) {
			cerr<<"\nyou should increase the average degree (bigger than "<<average_k1<<")"<<endl;
			cerr<<"(or decrease the maximum degree...)"<<endl;
		}
		if (average_k2-dmed<0) {
			cerr<<"\nyou should decrease the average degree (smaller than "<<average_k2<<")"<<endl;
			cerr<<"(or increase the maximum degree...)"<<endl;
		}
		return -1;
	}
    
	while (fabs(average_k1-dmed)>1e-7) {
		double temp=average_degree(dmax, ((dmin_r+dmin_l)/2.), gamma);
		if ((temp-dmed)*(average_k2-dmed)>0) {
			average_k2=temp;
			dmin_r=((dmin_r+dmin_l)/2.);
		}
		else {
			average_k1=temp;
			dmin_l=((dmin_r+dmin_l)/2.);
		}
        
	}
	
	return dmin_l;
}


#endif
