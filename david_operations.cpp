#include "david_operations.h";

inline double ops::sum(const alglib::real_1d_array& a)
{
	double result = .0f;
	const alglib::ae_int_t l = a.length();

	for (alglib::ae_int_t i = 0; i < l; i++)
		result += a[i];

	return result;
}

double ops::dot(const alglib::real_1d_array& a, const alglib::real_1d_array& b)
{
	alglib::real_1d_array pointWiseMult = operate(a, b, multiply);
	return sum(pointWiseMult);
}

double ops::dot(const double * const a, const double * const b, const size_t dims)
{
	alglib::real_1d_array _a = ops::makeArr(a, dims), _b = ops::makeArr(b, dims);
	return ops::dot(_a, _b);
}

alglib::real_1d_array ops::operate(const alglib::real_1d_array& a, const alglib::real_1d_array& b, double operation(const double&, const double&))
{
	const alglib::ae_int_t l = std::max(a.length(), b.length());
	alglib::real_1d_array c = ops::makeArr(l,false);

	for (alglib::ae_int_t i = 0; i < l; i++)
		c[i] = operation(a[i], b[i]);

	return c;
}

alglib::real_1d_array& ops::zero(alglib::real_1d_array& a)
{
	const alglib::ae_int_t l = a.length();
	for (alglib::ae_int_t i = 0; i < l; i++)
		a[i] = 0;

	return a;
}

alglib::real_1d_array ops::mean(const alglib::real_2d_array & points)
{
	alglib::ae_int_t dims = points.cols(), n = points.rows();
	alglib::real_1d_array mean = ops::makeArr(dims, true);
	
	// sum
	for (alglib::ae_int_t i = 0; i < n; i++)
		for (size_t d = 0; d < dims; d++)
			mean[d] += points[i][d];

	// divide
	for (alglib::ae_int_t d = 0; d < dims; d++)
		mean[d] /= n;

	return mean;
}

alglib::real_1d_array ops::makeArr(const alglib::ae_int_t n, bool zero)
{
	alglib::real_1d_array arr;
	arr.setlength(n);
	if (zero) ops::zero(arr);
	return arr;
}

alglib::real_1d_array ops::makeArr(const double * const content, const alglib::ae_int_t n)
{
	alglib::real_1d_array arr;
	arr.setcontent(n, content);
	return arr;
}

double* ops::flip(double * const v, const size_t dims)
{
	for (size_t i = 0; i < dims; i++)
		v[i] *= -1;

	return v;
}

alglib::real_1d_array & ops::flip(alglib::real_1d_array & v)
{
	const size_t dims = v.length();

	for (size_t i = 0; i < dims; i++)
		v[i] *= -1;

	return v;
}

alglib::real_1d_array operator+(const alglib::real_1d_array & a, const alglib::real_1d_array & b)
{
	return ops::operate(a, b, ops::add);
}

alglib::real_1d_array operator*(const alglib::real_1d_array & a, const alglib::real_1d_array & b)
{
	return ops::operate(a, b, ops::multiply);
}

alglib::real_1d_array operator-(const alglib::real_1d_array & a, const alglib::real_1d_array & b)
{
	return ops::operate(a, b, ops::subtract);
}


alglib::real_1d_array ops::getCentroid(const alglib::real_2d_array& points)
{
	return ops::mean(points);
}

alglib::real_1d_array ops::getNormal(const alglib::real_2d_array& points)
{
	alglib::ae_int_t dims = points.cols(), n = points.rows();

	// will store normal estimation to return
	alglib::real_1d_array normal;

	// pcaInfo returns 1 if valid
	alglib::ae_int_t pcaInfo;

	// pcaS2 -> eigenvalues in descending order
	// pcaV  -> eigenvectors in corresponding order
	alglib::real_1d_array pcaS2;
	alglib::real_2d_array pcaV;

	// perform full pca on the points
	alglib::pcabuildbasis(points, n, dims, pcaInfo, pcaS2, pcaV);

	// set normal to last eigenvector
	normal.setcontent(dims, pcaV[dims - 1]);

	return normal;
}

double* ops::copyArr(double* dest, alglib::real_1d_array& source)
{
	alglib::ae_int_t l = source.length();
	double * _source = source.getcontent();
	for (size_t d = 0; d < l; d++)
		dest[d] = _source[d];

	return dest;
}
