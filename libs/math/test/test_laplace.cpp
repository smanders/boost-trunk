//  Copyright Thijs van den Berg, 2008.
//  Copyright John Maddock 2008.
//  Copyright Paul A. Bristow 2008, 2009.

//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// TODO ?? PAB 2 Dec 2008

// add few more tests for farther out than 2??
// long double test
// add test for convenience typedef laplace.

/*

This module tests the Laplace distribution.

Test 1: test_pdf_cdf_ocatave()
         Compare pdf, cdf agains results obtained from GNU Octave.

Test 2: test_cdf_quantile_symmetry()
         Checks if quantile is the inverse of cdf by testing
         quantile(cdf(x)) == x

Test 3: test_hazard_pdf_cdf_symmetry()
         Checks the relation between hazard, pdf and cdf.
         hazard = pdf/(1-cdf)


Test 4: test_location_scale_symmetry()
         Checks the pdf, cdf invariant for translation and scaling invariant
                 cdf(x,location,scale) = cdf( (x-location)/scale, 0, 1)
         scale * pdf(x,location,scale) = pdf( (x-location)/scale, 0, 1)

Test 5: test_mmm_moments()
         Tests...
         mean   == location
         mode   == location
         median == location

         standard_deviation = sqrt(2)*scale
         skewness == 0
         kurtoris == 6
         excess kurtoris == 3

Test 6: test_complemented()
         Test the cdf an quantile complemented function.
         cdf(L,x) + cdf(complement(l,x)) == 1
         quantile(L,p) == quantile(complement(l,1-p))

Test 7: test_bad_dist_parameters()
         Test invalid distribution construction.

Test 8: test_extreme_function_arguments()
         Test x = +/- inf. for cdf(), pdf()
         Test p ={0,1} for quantile()
*/


#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/math/distributions/laplace.hpp>
#include "test_out_of_range.hpp"
using boost::math::laplace_distribution;

/*
#include <iostream>
   using std::cout;
   using std::endl;
   using std::setprecision;

#include <limits>
  using std::numeric_limits;
*/
   
/* 
   This function test various values of the standard Laplace distribution pdf,cdf
   against values generated by GNU Octave
   The test code is generated woth the following Octave script:
 
f = fopen('octave_boost_laplace.cpp', 'w');

for x = [real(-2.0), real(-1.0), real(-0.5), real(0.0), real(0.5), real(1.0), real(2.0)]

   # pdf tests
   # -----------------------
   fdisp(f, "   BOOST_CHECK_CLOSE(" ),;
   fdisp(f, (sprintf ("      pdf(laplace_distribution<RealType>(), static_cast<RealType>(%16.14fL)),", x)));
   fdisp(f, (sprintf ("      static_cast<RealType>(%16.14fL),", laplace_pdf(x) )) );
   fdisp(f, "   tolerance);" );
   fdisp(f, "" );
  
   # cdf tests
   # -----------------------
   fdisp(f, "   BOOST_CHECK_CLOSE(" );
   fdisp(f, (sprintf ("      cdf(laplace_distribution<RealType>(), static_cast<RealType>(%16.14fL)),", x)));
   fdisp(f, (sprintf ("      static_cast<RealType>(%16.14fL),", laplace_cdf(x) )) );
   fdisp(f, "   tolerance);" );
   fdisp(f, "" );
   
endfor

fclose(f);


Laplace distribution values version 2.0

Using NTL version 5.4 and the formula in Wikipedia Paul A. Bristow
NTL class RR precision 150 bits.
NTL class RR output precision 50 decimal digits OK.
Laplace pdf
-10  0.22699964881242425767795757780275305118959044437696e-4
-9.5  0.37425914943850295735594659677277583056493449453316e-4
-9  0.61704902043339774748818345365016913036076416123496e-4
-8.5  0.00010173418450532208718446671524353319364865576338509
-8  0.00016773131395125591941069456289043050965545006686111
-7.5  0.00027654218507391679155100004426517859890566829114007
-7  0.00045594098277725810400156804220464131323686226383407
-6.5  0.00075171959648878622369145166608382691500025647887521
-6  0.0012393760883331792115225837154083339457532397924584
-5.5  0.0020433857192320334967323513423603842041953283241185
-5  0.0033689734995427335483180242115742121244247925136568
-4.5  0.0055544982691211532480715671434652638857696337503689
-4  0.0091578194443670901468590106366206211059560337810038
-3.5  0.015098691711159250369893146181809922535830266119032
-3  0.024893534183931971489671207825030888315849796096331
-2.5  0.041042499311949397584764337233579903918902060509316
-2  0.067667641618306345946999747486242201703815772944649
-1.5  0.11156508007421491446664023538200626067108581471131
-1  0.18393972058572116079776188508073043372290556554506
-0.5  0.30326532985631671180189976749559022672095906778368
0  0.5
0.5  0.30326532985631671180189976749559022672095906778368
1  0.18393972058572116079776188508073043372290556554506
1.5  0.11156508007421491446664023538200626067108581471131
2  0.067667641618306345946999747486242201703815772944649
2.5  0.041042499311949397584764337233579903918902060509316
3  0.024893534183931971489671207825030888315849796096331
3.5  0.015098691711159250369893146181809922535830266119032
4  0.0091578194443670901468590106366206211059560337810038
4.5  0.0055544982691211532480715671434652638857696337503689
5  0.0033689734995427335483180242115742121244247925136568
5.5  0.0020433857192320334967323513423603842041953283241185
6  0.0012393760883331792115225837154083339457532397924584
6.5  0.00075171959648878622369145166608382691500025647887521
7  0.00045594098277725810400156804220464131323686226383407
7.5  0.00027654218507391679155100004426517859890566829114007
8  0.00016773131395125591941069456289043050965545006686111
8.5  0.00010173418450532208718446671524353319364865576338509
9  0.61704902043339774748818345365016913036076416123496e-4
9.5  0.37425914943850295735594659677277583056493449453316e-4
10  0.22699964881242425767795757780275305118959044437696e-4

Laplace cdf
-10  0.9999773000351187575742322042422197246948810411152
-9.5  0.99996257408505614970426440534032272241694350636029
-9  0.99993829509795666022525118165463498308696392404693
-8.5  0.99989826581549467791281553328475646680635134420916
-8  0.99983226868604874408058930543710956949034454956485
-7.5  0.9997234578149260832084489999557348214010943317417
-7  0.99954405901722274189599843195779535868676313746042
-6.5  0.99924828040351121377630854833391617308499974328643
-6  0.99876062391166682078847741628459166605424676032523
-5.5  0.99795661428076796650326764865763961579580467117776
-5  0.99663102650045726645168197578842578787557520756024
-4.5  0.9944455017308788467519284328565347361142303666328
-4  0.99084218055563290985314098936337937889404396651458
-3.5  0.98490130828884074963010685381819007746416973359633
-3  0.9751064658160680285103287921749691116841502037504
-2.5  0.95895750068805060241523566276642009608109793962206
-2  0.93233235838169365405300025251375779829618422688019
-1.5  0.8884349199257850855333597646179937393289141857266
-1  0.81606027941427883920223811491926956627709443427977
-0.5  0.69673467014368328819810023250440977327904093221632
0  0.5
0.5  0.30326532985631671180189976749559022672095906778368
1  0.18393972058572116079776188508073043372290556572023
1.5  0.11156508007421491446664023538200626067108581462372
2  0.067667641618306345946999747486242201703815773119812
2.5  0.041042499311949397584764337233579903918902060377944
3  0.024893534183931971489671207825030888315849796249598
3.5  0.015098691711159250369893146181809922535830266053346
4  0.009157819444367090146859010636620621105956033835742
4.5  0.005554498269121153248071567143465263885769633717526
5  0.0033689734995427335483180242115742121244247924397602
5.5  0.0020433857192320334967323513423603842041953284719117
6  0.0012393760883331792115225837154083339457532396747712
6.5  0.00075171959648878622369145166608382691500025636324071
7  0.00045594098277725810400156804220464131323686218925325
7.5  0.00027654218507391679155100004426517859890566825829713
8  0.00016773131395125591941069456289043050965545008482209
8.5  0.00010173418450532208718446671524353319364865579083973
9  0.61704902043339774748818345365016913036076303396971e-4
9.5  0.37425914943850295735594659677277583056493289386783e-4
10  0.22699964881242425767795757780275305118958884798806e-4

*/
template <class RealType>
void test_pdf_cdf_ocatave()
{
   RealType tolerance(1e-10f);
   
   BOOST_CHECK_CLOSE(
      pdf(laplace_distribution<RealType>(), static_cast<RealType>(-2.L)),
     // static_cast<RealType>(0.06766764161831L),
      static_cast<RealType>(0.067667641618306345946999747486242201703815773119812L),
   tolerance);

   BOOST_CHECK_CLOSE(
      cdf(laplace_distribution<RealType>(), static_cast<RealType>(-2.L)),
      //static_cast<RealType>(0.06766764161831L),
      static_cast<RealType>(0.067667641618306345946999747486242201703815773119812L),
   tolerance);

   BOOST_CHECK_CLOSE(
      pdf(laplace_distribution<RealType>(), static_cast<RealType>(-1.L)),
      //static_cast<RealType>(0.18393972058572L),
      static_cast<RealType>(0.18393972058572116079776188508073043372290556554506L),
   tolerance);

   BOOST_CHECK_CLOSE(
      cdf(laplace_distribution<RealType>(), static_cast<RealType>(-1.L)),
      static_cast<RealType>(0.18393972058572L),
   tolerance);

   BOOST_CHECK_CLOSE(
      pdf(laplace_distribution<RealType>(), static_cast<RealType>(-0.5L)),
   // static_cast<RealType>(0.30326532985632L),
      static_cast<RealType>(0.30326532985631671180189976749559022672095906778368L),
   tolerance);

   BOOST_CHECK_CLOSE(
      cdf(laplace_distribution<RealType>(), static_cast<RealType>(-0.5L)),
      //static_cast<RealType>(0.30326532985632L),
      static_cast<RealType>(0.30326532985631671180189976749559022672095906778368L),
   tolerance);

   BOOST_CHECK_CLOSE(
      pdf(laplace_distribution<RealType>(), static_cast<RealType>(0.0L)),
      static_cast<RealType>(0.5L),
   tolerance);

   BOOST_CHECK_CLOSE(
      cdf(laplace_distribution<RealType>(), static_cast<RealType>(0.0L)),
      static_cast<RealType>(0.5L),
   tolerance);

   BOOST_CHECK_CLOSE(
      pdf(laplace_distribution<RealType>(), static_cast<RealType>(0.5L)),
      //static_cast<RealType>(0.30326532985632L),
      static_cast<RealType>(0.30326532985631671180189976749559022672095906778368L),
   tolerance);

   BOOST_CHECK_CLOSE(
      cdf(laplace_distribution<RealType>(), static_cast<RealType>(0.5L)),
      // static_cast<RealType>(0.69673467014368L),
      static_cast<RealType>(0.69673467014368328819810023250440977327904093221632L),
   tolerance);

   BOOST_CHECK_CLOSE(
      pdf(laplace_distribution<RealType>(), static_cast<RealType>(1.0L)),
    //  static_cast<RealType>(0.18393972058572L),
      static_cast<RealType>(0.18393972058572116079776188508073043372290556554506L),
   tolerance);

   BOOST_CHECK_CLOSE(
      cdf(laplace_distribution<RealType>(), static_cast<RealType>(1.00000000000000L)),
     // static_cast<RealType>(0.81606027941428L),
      static_cast<RealType>(0.81606027941427883920223811491926956627709443427977L),
   tolerance);

   BOOST_CHECK_CLOSE(
      pdf(laplace_distribution<RealType>(), static_cast<RealType>(2.0L)),
    //  static_cast<RealType>(0.06766764161831L),
      static_cast<RealType>(0.067667641618306345946999747486242201703815772944649L),
   tolerance);

   BOOST_CHECK_CLOSE(
      cdf(laplace_distribution<RealType>(), static_cast<RealType>(2.0L)),
   //   static_cast<RealType>(0.93233235838169L),
      static_cast<RealType>(0.93233235838169365405300025251375779829618422688019L),
   tolerance);

   check_out_of_range<laplace_distribution<RealType> >(0, 1);
   BOOST_CHECK_THROW(laplace_distribution<RealType>(0, 0), std::domain_error);
   BOOST_CHECK_THROW(laplace_distribution<RealType>(0, -1), std::domain_error);
}

template <class RealType>
void test_cdf_quantile_symmetry()
{
   RealType tolerance(boost::math::tools::epsilon<RealType>() * 500); // 5 eps as a percentage

   const float xtest[7] = {  -2.0, -1.0, -0.5, 0.0, 0.5, 1.0, 2.0 };
   
   for (int i=0; i<7; ++i)
   {
      RealType x( static_cast<RealType>(xtest[i]) );
      RealType x2( quantile(laplace_distribution<RealType>(), cdf(laplace_distribution<RealType>(), x)) );
      BOOST_CHECK_CLOSE( x, x2, tolerance);
   }
}

template <class RealType>
void test_hazard_pdf_cdf_symmetry()
{
   RealType tolerance(boost::math::tools::epsilon<RealType>() * 500); // 5 eps as a percentage

   const float xtest[7] = {  -2.0, -1.0, -0.5, 0.0, 0.5, 1.0, 2.0 };

   for (int xi=0; xi<7; ++xi)
   {
      RealType x( static_cast<RealType>(xtest[xi]) );
      RealType p(  pdf(laplace_distribution<RealType>(), x)  );
      RealType c(  cdf(laplace_distribution<RealType>(), x)  );
      RealType h1(   p/(static_cast<RealType>(1.0) - c)  );
      RealType h2(  hazard(laplace_distribution<RealType>(), x)  );
      BOOST_CHECK_CLOSE( h1, h2, tolerance);
   }
}

template <class RealType>
void test_location_scale_symmetry()
{
   RealType tolerance(boost::math::tools::epsilon<RealType>() * 500); // 5 eps as a percentage

   const float xtest[7] = {  -2.0, -1.0, -0.5, 0.0, 0.5, 1.0, 2.0 };
   const float ltest[7] = {  -2.0, -1.0, -0.5, 0.0, 0.5, 1.0, 2.0 };
   const float stest[3] = {  0.5, 1.0, 2.0 };

   for (int xi=0; xi<7; ++xi)
      for (int li=0; li<7; ++li)
         for (int si=0; si<3; ++si)
         {
            RealType x( static_cast<RealType>(xtest[xi]) );
            RealType l( static_cast<RealType>(ltest[li]) );
            RealType s( static_cast<RealType>(stest[si]) );
            RealType x0( (x-l)/s );

            BOOST_CHECK_CLOSE( 
               pdf(laplace_distribution<RealType>(l,s), x) * s,
               pdf(laplace_distribution<RealType>(), x0),
               tolerance);

            BOOST_CHECK_CLOSE( 
               cdf(laplace_distribution<RealType>(l,s), x),
               cdf(laplace_distribution<RealType>(), x0),
               tolerance);

         }
}

template <class RealType>
void test_mmm_moments()
{
   RealType tolerance(boost::math::tools::epsilon<RealType>() * 500); // 5 eps as a percentage

 //  const float xtest[7] = {  -2.0, -1.0, -0.5, 0.0, 0.5, 1.0, 2.0 };
   const float ltest[7] = {  -2.0, -1.0, -0.5, 0.0, 0.5, 1.0, 2.0 };
   const float stest[3] = {  0.5, 1.0, 2.0 };

   for (int xi=0; xi<7; ++xi)
      for (int li=0; li<7; ++li)
         for (int si=0; si<3; ++si)
         {
            //RealType x( static_cast<RealType>(xtest[xi]) );
            RealType l( static_cast<RealType>(ltest[li]) );
            RealType s( static_cast<RealType>(stest[si]) );

            BOOST_CHECK_CLOSE( 
               mean( laplace_distribution<RealType>(l,s) ),
               l,
               tolerance);

            BOOST_CHECK_CLOSE( 
               median( laplace_distribution<RealType>(l,s) ),
               l,
               tolerance);

            BOOST_CHECK_CLOSE( 
               mode( laplace_distribution<RealType>(l,s) ),
               l,
               tolerance);


            BOOST_CHECK_CLOSE( 
               standard_deviation( laplace_distribution<RealType>(l,s) ),
               static_cast<RealType>( s * boost::math::constants::root_two<RealType>() ),
               tolerance);

            BOOST_CHECK_CLOSE( 
               skewness( laplace_distribution<RealType>(l,s) ),
               static_cast<RealType>(0),
               tolerance);

            BOOST_CHECK_CLOSE( 
               kurtosis( laplace_distribution<RealType>(l,s) ),
               static_cast<RealType>(6),
               tolerance);

            BOOST_CHECK_CLOSE( 
               kurtosis_excess( laplace_distribution<RealType>(l,s) ),
               static_cast<RealType>(3),
               tolerance);
         }
} // template <class RealType> void test_mmm_moments()

template <class RealType>
void test_complemented()
{
   RealType tolerance(boost::math::tools::epsilon<RealType>() * 500); // 5 eps as a percentage.

   const float xtest[7] = {  -2.0, -1.0, -0.5, 0.0, 0.5, 1.0, 2.0 }; // x values.
   const float ptest[7] = {  0.125, 0.25, 0.5, 0.75, 0.875 }; // probability values.
   const float ltest[7] = {  -2.0, -1.0, -0.5, 0.0, 0.5, 1.0, 2.0 }; // locations.
   const float stest[3] = {  0.5, 1.0, 2.0 }; // scales.

   for (int li=0; li<7; ++li)
      for (int si=0; si<3; ++si)
      {
         RealType l( static_cast<RealType>(ltest[li]) );
         RealType s( static_cast<RealType>(stest[si]) );

         for (int xi=0; xi<7; ++xi)
         {
            RealType x( static_cast<RealType>(xtest[xi]) );

            // Check sum of cdf and complement = unity.
            BOOST_CHECK_CLOSE( 
               cdf(complement(laplace_distribution<RealType>(l,s), x))
                 + cdf(laplace_distribution<RealType>(l,s), x),
               static_cast<RealType>(1),
               tolerance);

         }

         for (int pi=0; pi<5; ++pi)
         {
            RealType p( static_cast<RealType>(ptest[pi]) );

            BOOST_CHECK_CLOSE( 
               quantile(complement(laplace_distribution<RealType>(l,s), 1-p )), 
               quantile(laplace_distribution<RealType>(l,s), p),
               tolerance);
         }
      }
} // void test_complemented()

template <class RealType>
void test_bad_dist_parameters()
{
   // Check that can generate laplace distribution using both convenience methods:
   laplace_distribution<double> lp1(0.5); // Using default RealType double.
   boost::math::laplace lp2(0.5); // Using typedef. 

   BOOST_CHECK_THROW(boost::math::laplace_distribution<RealType> lbad1(0, 0), std::domain_error);
   BOOST_CHECK_THROW(boost::math::laplace_distribution<RealType> lbad2(0, -1), std::domain_error);
}

template <class RealType>
void test_extreme_function_arguments()
{
   using boost::math::policies::policy;
   using boost::math::policies::overflow_error; 
   using boost::math::policies::ignore_error;

   typedef policy<
      overflow_error<ignore_error> // if argument value causes overflow.
      > ignore_overflow_policy;

   boost::math::laplace_distribution<RealType, ignore_overflow_policy> L1(0, 1);
   boost::math::laplace_distribution<RealType, ignore_overflow_policy> L2(1, 2);
   // check pdf at x = +/- infinity
   BOOST_CHECK_THROW(pdf(L1, +std::numeric_limits<RealType>::infinity()), std::domain_error);
   BOOST_CHECK_THROW(pdf(L1, -std::numeric_limits<RealType>::infinity()), std::domain_error);
   BOOST_CHECK_THROW(pdf(L2, +std::numeric_limits<RealType>::infinity()), std::domain_error);
   BOOST_CHECK_THROW(pdf(L2, -std::numeric_limits<RealType>::infinity()), std::domain_error);

   // check cdf at x = +/- infinity
   BOOST_CHECK_THROW(cdf(L1, +std::numeric_limits<RealType>::infinity()), std::domain_error);
   BOOST_CHECK_THROW(cdf(L1, -std::numeric_limits<RealType>::infinity()), std::domain_error);
   BOOST_CHECK_THROW(cdf(L2, +std::numeric_limits<RealType>::infinity()), std::domain_error);
   BOOST_CHECK_THROW(cdf(L2, -std::numeric_limits<RealType>::infinity()), std::domain_error);


   // check quantile at p = 0, 1 which return infinity.
   BOOST_CHECK_EQUAL(quantile(L1, 0), -std::numeric_limits<RealType>::infinity() );
   BOOST_CHECK_EQUAL(quantile(L1, 1), +std::numeric_limits<RealType>::infinity() );
   BOOST_CHECK_EQUAL(quantile(L2, 0), -std::numeric_limits<RealType>::infinity() );
   BOOST_CHECK_EQUAL(quantile(L2, 1), +std::numeric_limits<RealType>::infinity() );
}

BOOST_AUTO_TEST_CASE( vs_GNU_Octave )
{
   test_pdf_cdf_ocatave<float>();
   test_pdf_cdf_ocatave<double>();
}

BOOST_AUTO_TEST_CASE( cdf_quantile_symmetry )
{
   test_cdf_quantile_symmetry<float>();
   test_cdf_quantile_symmetry<double>();
}


BOOST_AUTO_TEST_CASE( hazard_pdf_cdf_symmetry )
{
   test_hazard_pdf_cdf_symmetry<float>();
   test_hazard_pdf_cdf_symmetry<double>();
}

BOOST_AUTO_TEST_CASE( location_scale_symmetry )
{
   test_location_scale_symmetry<float>();
   test_location_scale_symmetry<double>();
}

BOOST_AUTO_TEST_CASE( mmm_moments )
{
   test_mmm_moments<float>();
   test_mmm_moments<double>();
}

BOOST_AUTO_TEST_CASE( t_complemented )
{
   test_complemented<float>();
   test_complemented<double>();
}

BOOST_AUTO_TEST_CASE( bad_dist_parameters )
{
   test_bad_dist_parameters<float>();
   test_bad_dist_parameters<double>();
}

BOOST_AUTO_TEST_CASE( extreme_function_arguments )
{
   test_extreme_function_arguments<float>();
   test_extreme_function_arguments<double>();
}

/*

Output:

------ Rebuild All started: Project: test_laplace, Configuration: Debug Win32 ------
  test_laplace.cpp
  test_laplace.vcxproj -> J:\Cpp\MathToolkit\test\Math_test\Debug\test_laplace.exe
  Running 8 test cases...
  
  *** No errors detected
========== Rebuild All: 1 succeeded, 0 failed, 0 skipped ==========


*/

