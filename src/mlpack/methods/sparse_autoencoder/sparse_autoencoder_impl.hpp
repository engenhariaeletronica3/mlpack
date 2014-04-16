namespace mlpack {
namespace nn {

template<template<typename> class OptimizerType>
SparseAutoencoder<OptimizerType>::SparseAutoencoder(const arma::mat& data,
                                                    const size_t visibleSize,
                                                    const size_t hiddenSize,
                                                    double lambda,
                                                    double beta,
                                                    double rho) :
    visibleSize(visibleSize),
    hiddenSize(hiddenSize),
    lambda(lambda),
    beta(beta),
    rho(rho)
{
  SparseAutoencoderFunction encoderFunction(data, visibleSize, hiddenSize,
                                            lambda, beta, rho);
  OptimizerType<SparseAutoencoderFunction> optimizer(encoderFunction);
  
  parameters = encoderFunction.GetInitialPoint();
  
  // Train the model.
  Timer::Start("sparse_autoencoder_optimization");
  const double out = optimizer.Optimize(parameters);
  Timer::Stop("sparse_autoencoder_optimization");

  Log::Info << "SparseAutoencoder::SparseAutoencoder(): final objective of "
      << "trained model is " << out << "." << std::endl;
}

template<template<typename> class OptimizerType>
SparseAutoencoder<OptimizerType>::SparseAutoencoder(
    OptimizerType<SparseAutoencoderFunction> &optimizer) :
    parameters(optimizer.Function().GetInitialPoint()),
    visibleSize(optimizer.Function().VisibleSize()),
    hiddenSize(optimizer.Function().HiddenSize()),
    lambda(optimizer.Function().Lambda()),
    beta(optimizer.Function().Beta()),
    rho(optimizer.Function().Rho())
{
  Timer::Start("sparse_autoencoder_optimization");
  const double out = optimizer.Optimize(parameters);
  Timer::Stop("sparse_autoencoder_optimization");

  Log::Info << "SparseAutoencoder::SparseAutoencoder(): final objective of "
      << "trained model is " << out << "." << std::endl;
}

template<template<typename> class OptimizerType>
void SparseAutoencoder<OptimizerType>::GetNewFeatures(arma::mat& data,
                                                      arma::mat& features)
{
  const size_t l1 = hiddenSize;
  const size_t l2 = visibleSize;

  features = Sigmoid(parameters.submat(0, 0, l1-1, l2-1) * data +
      arma::repmat(parameters.submat(0, l2, l1-1, l2), 1, data.n_cols));
}

}; // namespace nn
}; // namespace mlpack
