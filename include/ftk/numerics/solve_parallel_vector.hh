#ifndef _FTK_PARALLEL_VECTOR_H
#define _FTK_PARALLEL_VECTOR_H

#include <ftk/numerics/invmat.hh>
#include <ftk/numerics/eig.hh>
// #include <Eigen/Core>
// #include <Eigen/Eigenvalues>

namespace ftk {

template <typename ValueType>
inline bool isnan_mat3x3(const ValueType m[])
{
  for (int i=0; i<9; i++) 
    if (isnan(m[i])) return true;
  return false;
}

template <typename ValueType>
inline bool solve_parallel_vector_barycentric(const ValueType V[9], const ValueType W[9], ValueType lambda[3])
{
  ValueType invV[9], invW[9];
  const auto detV = invmat3(V, invV),
             detW = invmat3(W, invW);

  // print_mat3x3("V", V);
  // print_mat3x3("W", W);
  // fprintf(stderr, "devV=%f, devW=%f\n", detV, detW);
  if (isnan_mat3x3(invW) || isnan_mat3x3(invV)) return false;
  // if (detW < 1e-4) return false;

  ValueType m[9];
  mulmat3(invW, V, m);
  // if (detW > detV) mulmat3(invW, V, m); 
  // else mulmat3(invV, W, m); // very wrong...

#if 0
  print_mat3x3("V", V);
  print_mat3x3("invV", invV);
  print_mat3x3("W", W);
  print_mat3x3("invW", invW);
  print_mat3x3("invW*V", m);
#endif

  std::complex<ValueType> eig[3], eigvec[3][3];
  eig3(m, eig, eigvec);

  // fprintf(stderr, "eigs_impl0: (%f, %f), (%f, %f), (%f, %f)\n", 
  //     eig[0].real(), eig[0].imag(), eig[1].real(), eig[1].imag(), eig[2].real(), eig[2].imag());

  for (int i=0; i<3; i++) {
    if (isnan(eig[i].real()) || eig[i].imag() != 0) continue; // non-real eigenvalue
    ValueType l[3] = {eigvec[i][0].real(), eigvec[i][1].real(), eigvec[i][2].real()};
    if (l[0] < 0 || l[1] < 0) continue;
    const auto sum = l[0] + l[1] + l[2];
    lambda[0] = l[0] / sum;
    lambda[1] = l[1] / sum;
    lambda[2] = l[2] / sum;
    if (isnan(lambda[0]) || isnan(lambda[1]) || isnan(lambda[2])) continue;

#if 0
    print_mat3x3("V", V);
    print_mat3x3("invV", invV);
    print_mat3x3("W", W);
    print_mat3x3("invW", invW);
    print_mat3x3("invW*V", m);
#endif

    return true;
  }

  return false;
}

template <typename ValueType>
inline bool solve_parallel_vector_bilinear(const ValueType V[12], const ValueType W[12], ValueType lambda[2])
{
  // TODO
}

#if 0
template <typename ValueType>
inline bool parallel_vector1(const ValueType V[9], const ValueType W[9], ValueType lambda[3])
{
  Eigen::Matrix<ValueType, 3, 3> V1, W1;
  V1 << V[0], V[1], V[2], V[3], V[4], V[5], V[6], V[7], V[8];
  W1 << W[0], W[1], W[2], W[3], W[4], W[5], W[6], W[7], W[8];

  // print_mat3x3("V", V);
  // print_mat3x3("W", W);

  if (V1.determinant() == 0 || W1.determinant() == 0) return false;

  Eigen::GeneralizedEigenSolver<Eigen::Matrix<ValueType, 3, 3> > ges;
  ges.compute(V1, W1);
#if 0 
  lambda[0] = lambda[1] = lambda[2] = 0;
  auto b = parallel_vector(V, W, lambda);
  fprintf(stderr, "%d, %f, %f, %f\n", b, lambda[0], lambda[1], lambda[2]);
  
  fprintf(stderr, "eigs_impl1: (%f, %f), (%f, %f), (%f, %f)\n", 
      ges.eigenvalues()(0).real(), ges.eigenvalues()(0).imag(), 
      ges.eigenvalues()(1).real(), ges.eigenvalues()(1).imag(), 
      ges.eigenvalues()(2).real(), ges.eigenvalues()(2).imag());
  std::cerr << "eigvectors_impl1(0):\n" << ges.eigenvectors().col(0) << std::endl;
  std::cerr << "eigvectors_impl1(1):\n" << ges.eigenvectors().col(1) << std::endl;
  std::cerr << "eigvectors_impl1(2):\n" << ges.eigenvectors().col(2) << std::endl;
#endif 
  
  // std::cout << "eigvecs:\n" << ges.eigenvectors() << std::endl;
  for (int i=0; i<3; i++) {
    const std::complex<ValueType> eig = ges.eigenvalues()(i);
    const Eigen::Matrix<std::complex<ValueType>, 3, 1> vec = ges.eigenvectors().col(i);

    if (eig.imag() != 0) continue;
    const ValueType sum = vec.sum().real(); // ges.eigenvectors().col(i)(0).real() + ges.eigenvectors().col(i)(1).real() + ges.eigenvectors().col(i)(2).real();
    lambda[0] = vec(0).real() / sum; 
    lambda[1] = vec(1).real() / sum;
    lambda[2] = vec(2).real() / sum;
    if (lambda[0] * lambda[1] < 0 || lambda[0] * lambda[2] < 0) continue;
    if (isnan(lambda[0]) || isnan(lambda[1]) || isnan(lambda[2])) continue;
    return true;

#if 0
    if (vec.imag().norm() == 0 && eig.imag() == 0) { // all real values
      // std::cout << "eigvec" << i << ":\n" << vec << std::endl;
      const ValueType sum = vec.real().sum();
      fprintf(stderr, "sum=%.12f\n", sum);
      // if (isnan(sum) || isinf(sum) || sum < 1e-6) continue;
      if (isnan(sum) || isinf(sum)) continue;
      lambda[0] = vec(0).real() / sum;
      lambda[1] = vec(1).real() / sum;
      lambda[2] = vec(2).real() / sum;
      if (lambda[0] * lambda[1] < 0 || lambda[0] * lambda[2] < 0) continue;
      // fprintf(stderr, "i=%d, sum=%f, lambda_impl1: %f, %f, %f\n", i, sum, lambda[0], lambda[1], lambda[2]);
      // std::cerr << "vec:" << vec << std::endl;
      return true;
    }
#endif
  }

  return false;
}
#endif

}
#endif