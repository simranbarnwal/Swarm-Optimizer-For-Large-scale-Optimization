#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h> 

// global variables
float *array;
float fitness(float x[3], float v[3]);
int find_minimum(float arr[], int n);
int cmp(const void *a, const void *b);
double r2();
int r1(int lower, int upper);
void delay(int milli_seconds);

int main()
{
    /* INPUTS: swarm size NP, number of levels NL, level size LS, maximum number of fitness evaluations MAX_FES, control parameter phi */
    int np, ls, max_fes, nl, numt, tid; 
    double phi;
	srand(time(0));
    printf("Enter swarm size = ");
    scanf("%d", &np);
    printf("Enter level size = ");
    scanf("%d", &ls);
    printf("Enter maximum number of fitness evaluations = ");
    scanf("%d", &max_fes);
    printf("Enter control parameter ( works like learning rate, between 0 and 1, preferably around 0.1 to 0.2 to prevent overshooting) = ");
    scanf("%f", &phi);

	clock_t start, end; // to find time taken by parallel code
	double cpu_time_used;
	start = clock();

    nl = (np%ls==0) ? (int)np/ls:((int)np/ls+1);
    printf("number of levels = %d \n", nl);
    int l[nl+1];
    l[0]=-1;
    l[1] = (np%ls==0) ? (ls-1):((np%ls)-1);
    //printf("The levels are marked by these indices\n [ %d, ",l[1]);

    // the level dividers
    for(int i=2; i<nl; i++)
    {
        l[i] = l[i-1]+ls;
        //printf("%d, ",l[i]);
    }
    //printf("]\n\n");

    /* first we randomly initialize the positions and velocity of the swarm particles */
      // velocity
	float x[np][3]; // position
    float v[np][3]; // velocity

    /* these are the upper and lower bounds on x and v which can be changed */
    float lbx[3] = {0.0, 0.0, 0.0};
    float ubx[3] = {30, 30, 30};
    float lbv[3] = {-2, -2, -2};
    float ubv[3] = {2, 2, 2};

    int fes = 0; // number of fitness evaluations
    float f[np]; // the fitness of particles and their indices
    int index[np];


    for(int i=0; i<np; i++)
    {
        for(int j=0; j<3; j++)
        {
            x[i][j] = r1(lbx[j],ubx[j]);
            v[i][j] = r1(lbv[j],ubv[j]);
        }

        /* calculating fitness of the particles */
        f[i] = fitness(x[i], v[i]);
        index[i] = i;
        // printf("x, v and fitness of %d th particle is (%.1f, %.1f, %.1f), (%.1f, %.1f, %.1f) and %.2f\n",i+1,x[i][0],x[i][1],x[i][2],v[i][0],v[i][1], v[i][2], f[i]);
    }

    fes = fes + np; // because we have used fitness function np times in the previous loop

    /* now we find the best particle in terms of fitness ( the one with the lowest is the best as it is a minimization problem) */

    int xbest = find_minimum(f,np);
    printf("Fes = %d with best fitness = %f\n", fes, f[xbest]);

    /* here comes the main loop, which finds the global minima */
    int l1, l2;
    while(fes < max_fes)
    {       
		array = f;
        qsort(index, np, sizeof(*index), cmp);// index stores the particle id arranged according to fitness
        for(int i = nl; i>=3; i--)
        {
            for(int j = 1; j<=ls; j++)
            {
                // randomly select two higher levels
                l1 = r1(1,i-1);
                l2 = r1(1,i-1);
                if(l2<l1)
                {
                    int temp = l1;
                    l1 = l2;
                    l2 = temp;
                }

                // randomly select a particle in each level
                int x1 = index[r1(l[l1-1]+1,l[l1])];
                int x2 = index[r1(l[l2-1]+1,l[l2])];
                double a1 = r2();
                double a2 = r2();
                double a3 = r2();

                int xij = index[l[i-1]+j];
                v[xij][0] = a1*v[xij][0]+a2*(x[x1][0]-x[xij][0]) + a3*phi*(x[x2][0]-x[xij][0]);
                x[xij][0]=x[xij][0]+v[xij][0];
                v[xij][1] = a1*v[xij][1]+a2*(x[x1][1]-x[xij][1]) + a3*phi*(x[x2][1]-x[xij][1]);
                x[xij][1]=x[xij][1]+v[xij][1];
                v[xij][2] = a1*v[xij][2]+a2*(x[x1][2]-x[xij][2]) + a3*phi*(x[x2][2]-x[xij][2]);
                x[xij][2]=x[xij][2]+v[xij][2];


                f[xij]=fitness(x[xij],v[xij]);

                if(f[xij]<f[xbest])
                    xbest = xij;
            }
            fes = fes + ls;
        }

        // updating of second level
        for(int j = 1; j<=ls; j++)
        {

            // randomly select a particle in each level
            int x1 = index[r1(l[0]+1,l[1])];
            int x2 = index[r1(l[0]+1,l[1])];

            if(f[x2]<f[x1])
            {
                int temp = x1;
                x1 = x2;
                x2 = temp;
            }

            double a1 = r2();
            double a2 = r2();
            double a3 = r2();

            int x2j = index[l[1]+1+j];
            v[x2j][0] = a1*v[x2j][0]+a2*(x[x1][0]-x[x2j][0]) + a3*phi*(x[x2][0]-x[x2j][0]);
            x[x2j][0]=x[x2j][0]+v[x2j][0];
            v[x2j][1] = a1*v[x2j][1]+a2*(x[x1][1]-x[x2j][1]) + a3*phi*(x[x2][1]-x[x2j][1]);
            x[x2j][1]=x[x2j][1]+v[x2j][1];
            v[x2j][2] = a1*v[x2j][2]+a2*(x[x1][2]-x[x2j][2]) + a3*phi*(x[x2][2]-x[x2j][2]);
            x[x2j][2]=x[x2j][2]+v[x2j][2];


            f[x2j]=fitness(x[x2j],v[x2j]);

            if(f[x2j]<f[xbest])
                xbest = x2j;

        }
        fes = fes + ls;
        printf("Fes = %d with best fitness = %f\n", fes, f[xbest]);

    }
    printf("The best fitness is %f and the best particle is %d\n", f[xbest], xbest);
	end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Time taken by serial code = %f\n",cpu_time_used);
    return 0;
}

float fitness(float x[3], float v[3])
{
    /* x is the coordinate and v is the velocity of the particle */

    float of = 20*(pow((x[0]-1),2) + pow((x[1]-2),2) + pow((x[2]-3),2))+
				10*(pow((x[0]-0),2) + pow((x[1]-7),2) + pow((x[2]-8),2));

    /* this is the objective function that we want to solve, we can tweak this according to whatever we wish to minimize */

    /* the constraints or objective function can also use velocity components but we are keeping it simple */

    float c[2];

    /* here two is the number of constraints, we can change this according to the number of constraints we wish to add */

    c[0] = x[2] - 25; // this is constraint 1, a <= constraint
    c[1] = pow(x[0],2) + 2*x[1] - x[2]; // this is constraint 2, a <= constraint

    int viol = 0; // to count how many constraints are violated
    for( int i = 0; i <= sizeof(c)/sizeof(c[0]); i++)
    {
        if(c[i]>0)
            viol++;
    }

    float pen = 1000.0; // this is the penalty imposed on each constraint violation
    float fit = of + pen*viol;
	delay(50);
    return fit;

}

int find_minimum(float arr[],int n)
{
    int c, min, index;
    min = arr[0];
    index = 0;

    for (c = 1; c < n; c++)
    {
        if (arr[c] < min)
        {
            index = c;
            min = arr[c];
        }
    }
    return index;
}

int cmp(const void *a, const void *b)
{
    int ia = *(int *)a;
    int ib = *(int *)b;
    return array[ia] < array[ib] ? -1 : array[ia] > array[ib];
}
double r2() // random funtion which returns value between 0 and 1
{
    return ((double)rand() / (double)RAND_MAX) ;
}
int r1(int lower, int upper)// random function which returns integer between upper and lower
{
    return (lower + rand()%(upper-lower+1));
}
void delay(int milli_seconds) // for artificially making fitness evaluation costly
{ 
  
    // Stroing start time 
    clock_t start_time = clock(); 
  
    // looping till required time is not acheived 
    while (clock() < start_time + milli_seconds); 
} 

