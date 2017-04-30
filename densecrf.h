#pragma once

#include <vector>
#include <cstdlib>

class PairwisePotential{
public:
	virtual ~PairwisePotential();
	virtual void apply( float * out_values, const float * in_values, float * tmp, int value_size ) const = 0;
};
class SemiMetricFunction{
public:
	virtual ~SemiMetricFunction();
	// For two probabilities apply the semi metric transform: v_i = sum_j mu_ij u_j
	virtual void apply( float * out_values, const float * in_values, int value_size ) const = 0;
};


class DenseCRF{
protected:
	friend class BipartiteDenseCRF;
	
	// Number of variables and labels
	int N_, M_;
	float *unary_, *additional_unary_, *current_, *next_, *tmp_;
	
	// Store all pairwise potentials
	std::vector<PairwisePotential*> pairwise_;
	
	// Run inference and return the pointer to the result
	float* runInference( int n_iterations, float relax);
	
	// Auxillary functions
	void expAndNormalize( float* out, const float* in, float scale = 1.0, float relax = 1.0 );
	
	// Don't copy this object, bad stuff will happen
	DenseCRF( DenseCRF & o ){}
public:
	// Create a dense CRF model of size N with M labels
	DenseCRF( int N, int M );
	virtual ~DenseCRF();
	// Add  a pairwise potential defined over some feature space
	void addPairwiseEnergy( const float * features, int D, float w=1.0f, const SemiMetricFunction * function=NULL );
	
	// Add your own favorite pairwise potential (ownwership will be transfered to this class)
	void addPairwiseEnergy( PairwisePotential* potential );
	
	// Set the unary potential for all variables and labels (memory order is [x0l0 x0l1 x0l2 .. x1l0 x1l1 ...])
	void setUnaryEnergy( const float * unary );
	
	// Set the unary potential for a specific variable
	void setUnaryEnergy( int n, const float * unary );
	
	// Run inference and return the probabilities
	void inference( int n_iterations, float* result, float relax=1.0 );
	
	// Run MAP inference and return the map for each pixel
	void map( int n_iterations, short int* result, float relax=1.0 );
	
	// Step by step inference
	void startInference();
	void stepInference( float relax = 1.0 );
	void currentMap( short * result );
	
public: /* Debugging functions */
	// Compute the unary energy of an assignment
	void unaryEnergy( const short * ass, float * result );
	
	// Compute the pairwise energy of an assignment (half of each pairwise potential is added to each of it's endpoints)
	void pairwiseEnergy( const short * ass, float * result, int term=-1 );
};

class DenseCRF2D:public DenseCRF{
protected:
	// Width, height of the 2d grid
	int W_, H_;
public:
	// Create a 2d dense CRF model of size W x H with M labels
	DenseCRF2D( int W, int H, int M );
	virtual ~DenseCRF2D();
	// Add a Gaussian pairwise potential with standard deviation sx and sy
	void addPairwiseGaussian( float sx, float sy, float w, const SemiMetricFunction * function=NULL );
	
	// Add a Bilateral pairwise potential with spacial standard deviations sx, sy and color standard deviations sr,sg,sb
	void addPairwiseBilateral( float sx, float sy, float sr, float sg, float sb, const unsigned char * im, float w, const SemiMetricFunction * function=NULL );
	
	// Set the unary potential for a specific variable
	void setUnaryEnergy( int x, int y, const float * unary );
	using DenseCRF::setUnaryEnergy;
};

// A dense CRF in a bipartite graph
class BipartiteDenseCRF{
protected:
	// Two dense CRF's that are connected by a set of completely connected edges (in a bipartite graph)
	DenseCRF* dense_crfs_[2];
	
	// Number of variables and labels
	int N_[2], M_;
	
	// All bipartite pairwise potentials (all others are stored in each dense_crfs respectively)
	std::vector<PairwisePotential*> pairwise_[2];
	
	// Don't copy this object, bad stuff will happen
	BipartiteDenseCRF( BipartiteDenseCRF & o ){}
	
	// Run inference and return the pointer to the result
	void runInference( int n_iterations, float ** prob, float relax);
public:
	// Create a dense CRF model of size N with M labels
	BipartiteDenseCRF( int N1, int N2, int M );
	~BipartiteDenseCRF();
	
	// Add  a pairwise potential defined over some feature space
	// The potential will have the form:    w*exp(-0.5*|f_i - f_j|^2)
	// The kernel shape should be captured by transforming the
	// features before passing them into this function
	void addPairwiseEnergy( const float * features1, const float * features2, int D, float w=1.0f, const SemiMetricFunction * function=NULL );
	
	// Add your own favorite pairwise potential (ownwership will be transfered to this class)
	void addPairwiseEnergy( PairwisePotential* potential12, PairwisePotential* potential21 );
	
	// Run inference and return the probabilities
	void inference( int n_iterations, float* result1, float * result2, float relax=1 );
	
	// Run MAP inference and return the map for each pixel
	void map( int n_iterations, short int* result1, short int* result2, float relax=1 );
	
	// Access the two CRF's directly
	DenseCRF& getCRF( int i );
	const DenseCRF& getCRF( int i ) const;
	
	// Step by step inference
	void startInference();
	void stepInference( float relax = 1.0 );
	void currentMap( short * result );
};



// This function defines a simplified interface to the permutohedral lattice
// We assume a filter standard deviation of 1
class Permutohedral;
class Filter{
protected:
    int n1_, o1_, n2_, o2_;
    Permutohedral * permutohedral_;
    // Don't copy
    Filter( const Filter& filter ){}
public:
    // Use different source and target features
    Filter( const float * source_features, int N_source, const float * target_features, int N_target, int feature_dim );
    // Use the same source and target features
    Filter( const float * features, int N, int feature_dim );
    //
    ~Filter();
    // Filter a bunch of values
    void filter( const float * source, float * target, int value_size );
};
