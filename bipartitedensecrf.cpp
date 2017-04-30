#include "densecrf.h"
#include "util.h"

class BPPottsPotential: public PairwisePotential{
protected:
	Permutohedral lattice_;
	BPPottsPotential( const BPPottsPotential&o ){}
	int N1_, N2_;
	float w_;
	float *norm_;
public:
	~BPPottsPotential(){
		deallocate( norm_ );
	}
	BPPottsPotential(const float* features1, const float* features2, int D, int N1, int N2, float w, bool per_pixel_normalization=true) :N1_(N1), N2_(N2), w_(w) {
		float * features = new float[ (N1_+N2_)*D ];
		memset( features, 0, (N1_+N2_)*D*sizeof(float) );
		memcpy( features      , features1, N1_*D*sizeof(float) );
		memcpy( features+N1_*D, features2, N2_*D*sizeof(float) );
		lattice_.init( features, D, N1_+N2_ );
		delete [] features;
		
		norm_ = allocate( N2_ );
		float * tmp = allocate( N1_ );
		for( int i=0; i<N1_; i++ )
			tmp[i] = 1;
		// Compute the normalization factor
		lattice_.compute( norm_, tmp, 1, 0, N1_, N1_, N2_ );
		if( per_pixel_normalization ){
			// use a per pixel normalization
			for( int i=0; i<N2_; i++ )
				norm_[i] = 1.f / (norm_[i]+1e-20f);
		}
		else{
			float mean_norm = 0;
			for( int i=0; i<N2_; i++ )
				mean_norm += norm_[i];
			mean_norm = N2_ / mean_norm;
			// use a per pixel normalization
			for( int i=0; i<N2_; i++ )
				norm_[i] = mean_norm;
		}
		deallocate( tmp );
	}
	virtual void apply( float * out_values, const float * in_values, float * tmp, int value_size ) const{
		lattice_.compute( tmp, in_values, value_size, 0, N1_, N1_, N2_ );
		for( int i=0,k=0; i<N2_; i++ )
			for( int j=0; j<value_size; j++, k++ )
				out_values[k] += w_*norm_[i]*tmp[k];
	}
};
class BPSemiMetricPotential: public BPPottsPotential{
protected:
	const SemiMetricFunction * function_;
public:
	void apply(float* out_values, const float* in_values, float* tmp, int value_size) const {
		lattice_.compute( tmp, in_values, value_size, 0, N1_, N1_, N2_ );

		// To the metric transform
		float * tmp2 = new float[value_size];
		for ( int i=0; i<N2_; i++ ) {
			float * out = out_values + i*value_size;
			float * t1  = tmp  + i*value_size;                                                                  ;
			function_->apply( tmp2, t1, value_size );
			for ( int j=0; j<value_size; j++ )
				out[j] -= w_*norm_[i]*tmp2[j];
		}
		delete[] tmp2;
	}
	BPSemiMetricPotential(const float* features1, const float* features2, int D, int N1, int N2, float w, const SemiMetricFunction* function, bool per_pixel_normalization=true) :BPPottsPotential( features1, features2, D, N1, N2, w, per_pixel_normalization ),function_(function) {
	}
};


/************************************************  Bipartite Dense CRF  *****************************************/
BipartiteDenseCRF::BipartiteDenseCRF(int N1, int N2, int M): M_(M) {
	dense_crfs_[0] = new DenseCRF( N1, M );
	dense_crfs_[1] = new DenseCRF( N2, M );
	N_[0] = N1;
	N_[1] = N2;
}
BipartiteDenseCRF::~BipartiteDenseCRF() {
	for( int i=0; i<2; i++ ){
		delete dense_crfs_[i];
		for( unsigned int j=0; j<pairwise_[i].size(); j++ )
			delete pairwise_[i][j];
	}
}

/********************************************** Pairwise Potentials***********************************************************/
void BipartiteDenseCRF::addPairwiseEnergy (const float* features1, const float* features2, int D, float w, const SemiMetricFunction * function) {
	if (function)
		addPairwiseEnergy( new BPSemiMetricPotential( features1, features2, D, N_[0], N_[1], w, function ),
		                   new BPSemiMetricPotential( features2, features1, D, N_[1], N_[0], w, function ) );
	else
		addPairwiseEnergy( new BPPottsPotential( features1, features2, D, N_[0], N_[1], w ),
		                   new BPPottsPotential( features2, features1, D, N_[1], N_[0], w ) );
}
void BipartiteDenseCRF::addPairwiseEnergy ( PairwisePotential* potential12, PairwisePotential* potential21 ){
	pairwise_[0].push_back( potential12 );
	pairwise_[1].push_back( potential21 );
}

/*******************************************************Inference***************************************************************/
void BipartiteDenseCRF::inference ( int n_iterations, float* result1, float* result2, float relax ) {
	// Run inference
	float * prob[2];
	runInference( n_iterations, prob, relax );
	// Copy the result over
	float* r[2] = {result1, result2};
	for( int k=0; k<2; k++ )
		for( int i=0; i<N_[k]; i++ )
			memcpy( r[k]+i*M_, prob[k]+i*M_, M_*sizeof(float) );
}
void BipartiteDenseCRF::map ( int n_iterations, short* result1, short* result2, float relax ) {
	// Run inference
	float * prob[2];
	runInference( n_iterations, prob, relax );
	dense_crfs_[0]->currentMap( result1 );
	dense_crfs_[1]->currentMap( result2 );
}
void BipartiteDenseCRF::runInference( int n_iterations, float ** prob, float relax ) {
	// Initialize using the unary energies
	startInference();
	for( int it=0; it<n_iterations; it++ )
		stepInference( relax );
	for( int k=0; k<2; k++ )
		prob[k] = dense_crfs_[k]->current_;
}
void BipartiteDenseCRF::startInference(){
	// Initialize using the unary energies
	for( int k=0; k<2; k++ )
		dense_crfs_[k]->startInference();
}
void BipartiteDenseCRF::stepInference( float relax ){
	for( int k=0; k<2; k++ ){
		// Compute all "incoming" pairwise energies
		for( int i=0; i<N_[k]*M_; i++ )
			dense_crfs_[k]->additional_unary_[i] = 0;
		
		// Add up all pairwise potentials
		for( unsigned int i=0; i<pairwise_[!k].size(); i++ )
			pairwise_[!k][i]->apply( dense_crfs_[k]->additional_unary_, dense_crfs_[!k]->current_, dense_crfs_[k]->tmp_, M_ );
		
		for( int i=0; i<N_[k]*M_; i++ )
			dense_crfs_[k]->additional_unary_[i] = -dense_crfs_[k]->additional_unary_[i];
		
		dense_crfs_[k]->stepInference( relax );
	}
}
DenseCRF& BipartiteDenseCRF::getCRF(int i) {
	return *dense_crfs_[i];
}
const DenseCRF& BipartiteDenseCRF::getCRF(int i) const {
	return *dense_crfs_[i];
}
