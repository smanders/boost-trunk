#ifndef BOOST_PREPROCESSOR_WHILE_HPP
#define BOOST_PREPROCESSOR_WHILE_HPP

/* Copyright (C) 2001
 * Housemarque Oy
 * http://www.housemarque.com
 *
 * Permission to copy, use, modify, sell and distribute this software is
 * granted provided this copyright notice appears in all copies. This
 * software is provided "as is" without express or implied warranty, and
 * with no claim as to its suitability for any purpose.
 *
 * See http://www.boost.org for most recent version.
 */

#include <boost/preprocessor/if.hpp>
#include <boost/preprocessor/tuple/eat.hpp>

/** <p>Iterates <code>OP(D,STATE)</code> while <code>PRED(D,STATE)</code> is true.</p>

<p>In other words, expands to:</p>

<pre>
OP(D, ... OP(D, OP(D,STATE) ) ... )
</pre>

<p>The depth of iteration is determined by <code>PRED(D,STATE)</code>.</p>

<p>For example,</p>

<pre>
#define PRED(D,STATE) BOOST_PP_LESS_D(D,BOOST_PP_TUPLE_ELEM(2,0,STATE),BOOST_PP_TUPLE_ELEM(2,1,STATE))
#define OP(D,STATE) (BOOST_PP_INC(BOOST_PP_TUPLE_ELEM(2,0,STATE)),BOOST_PP_TUPLE_ELEM(2,1,STATE))
BOOST_PP_WHILE(PRED,OP,(0,3))
</pre>

<p>expands to:</p>

<pre>
(3,3)
</pre>

<h3>Legend</h3>
<ul>
  <li><b>STATE</b> is the current state of iteration. The state is usually a tuple.</li>
  <li><b>PRED</b> is the condition for iteration. It must expand to a decimal
      integer literal.</li>
  <li><b>OP</b> is the iterated macro. Note that if the state is a tuple, then
      OP(D,STATE) usually expands to a tuple of the same number of elements.</li>
  <li><b>D</b> is the recursion depth and should only be used as a parameter
      to other macros using BOOST_PP_WHILE(). Such macros include
      BOOST_PP_ADD() and other arithmetic operations. For each macro using
      BOOST_PP_WHILE(), there is a version of the macro, distinguished by the
      D suffix (e.g. BOOST_PP_ADD_D()), that accepts an additional recursion
      depth as the first parameter. This technique is necessary to avoid
      recursively expanding the same macro again, which is not permitted by the
      C++ preprocessor.</li>
</ul>

<h3>Automatic recursion?</h3>

<p>BOOST_PP_WHILE() currently does not implement automatic recursion. The reason
for this is that it would lead to very poor performance. The automatic recursion
technique takes O(N) steps just to find out that the Nth recursion should be used.
This would dramatically effect the time complexity of macros using automatic
recursion.</p>

<h3>Note</h3>
<ul>
  <li>The value of the D parameter may exceed BOOST_PP_LIMIT_MAG.</li>
  <li>Using BOOST_PP_WHILE() is a bit tricky. This is due to the C++
      preprocessor limitations. It is recommended to take a look at the
      implementations of the various PREPROCESSOR library primitives
      such as BOOST_PP_ADD() for additional examples.</li>
</ul>

<h3>Example</h3>
<ul>
  <li><a href="../../example/count_down.c">count_down.c</a></li>
  <li><a href="../../example/linear_fib.c">linear_fib.c</a></li>
  <li><a href="../../example/delay.c">delay.c</a></li>
</ul>
*/
#define BOOST_PP_WHILE(PRED,OP,STATE) BOOST_PP_WHILE_C(PRED(1,STATE),0,STATE)(PRED,OP,OP(1,STATE))

#define BOOST_PP_WHILE_C(C,D,S) BOOST_PP_IF(C,BOOST_PP_WHILE##D,S BOOST_PP_TUPLE3_EAT)
#define BOOST_PP_WHILE0(P,O,S) BOOST_PP_WHILE_C(P(2,S),1,S)(P,O,O(2,S))
#define BOOST_PP_WHILE1(P,O,S) BOOST_PP_WHILE_C(P(3,S),2,S)(P,O,O(3,S))
#define BOOST_PP_WHILE2(P,O,S) BOOST_PP_WHILE_C(P(4,S),3,S)(P,O,O(4,S))
#define BOOST_PP_WHILE3(P,O,S) BOOST_PP_WHILE_C(P(5,S),4,S)(P,O,O(5,S))
#define BOOST_PP_WHILE4(P,O,S) BOOST_PP_WHILE_C(P(6,S),5,S)(P,O,O(6,S))
#define BOOST_PP_WHILE5(P,O,S) BOOST_PP_WHILE_C(P(7,S),6,S)(P,O,O(7,S))
#define BOOST_PP_WHILE6(P,O,S) BOOST_PP_WHILE_C(P(8,S),7,S)(P,O,O(8,S))
#define BOOST_PP_WHILE7(P,O,S) BOOST_PP_WHILE_C(P(9,S),8,S)(P,O,O(9,S))
#define BOOST_PP_WHILE8(P,O,S) BOOST_PP_WHILE_C(P(10,S),9,S)(P,O,O(10,S))
#define BOOST_PP_WHILE9(P,O,S) BOOST_PP_WHILE_C(P(11,S),10,S)(P,O,O(11,S))
#define BOOST_PP_WHILE10(P,O,S) BOOST_PP_WHILE_C(P(12,S),11,S)(P,O,O(12,S))
#define BOOST_PP_WHILE11(P,O,S) BOOST_PP_WHILE_C(P(13,S),12,S)(P,O,O(13,S))
#define BOOST_PP_WHILE12(P,O,S) BOOST_PP_WHILE_C(P(14,S),13,S)(P,O,O(14,S))
#define BOOST_PP_WHILE13(P,O,S) BOOST_PP_WHILE_C(P(15,S),14,S)(P,O,O(15,S))
#define BOOST_PP_WHILE14(P,O,S) BOOST_PP_WHILE_C(P(16,S),15,S)(P,O,O(16,S))
#define BOOST_PP_WHILE15(P,O,S) BOOST_PP_WHILE_C(P(17,S),16,S)(P,O,O(17,S))
#define BOOST_PP_WHILE16(P,O,S) BOOST_PP_WHILE_C(P(18,S),17,S)(P,O,O(18,S))
#define BOOST_PP_WHILE17(P,O,S) BOOST_PP_WHILE_C(P(19,S),18,S)(P,O,O(19,S))
#define BOOST_PP_WHILE18(P,O,S) BOOST_PP_WHILE_C(P(20,S),19,S)(P,O,O(20,S))
#define BOOST_PP_WHILE19(P,O,S) BOOST_PP_WHILE_C(P(21,S),20,S)(P,O,O(21,S))
#define BOOST_PP_WHILE20(P,O,S) BOOST_PP_WHILE_C(P(22,S),21,S)(P,O,O(22,S))
#define BOOST_PP_WHILE21(P,O,S) BOOST_PP_WHILE_C(P(23,S),22,S)(P,O,O(23,S))
#define BOOST_PP_WHILE22(P,O,S) BOOST_PP_WHILE_C(P(24,S),23,S)(P,O,O(24,S))
#define BOOST_PP_WHILE23(P,O,S) BOOST_PP_WHILE_C(P(25,S),24,S)(P,O,O(25,S))
#define BOOST_PP_WHILE24(P,O,S) BOOST_PP_WHILE_C(P(26,S),25,S)(P,O,O(26,S))
#define BOOST_PP_WHILE25(P,O,S) BOOST_PP_WHILE_C(P(27,S),26,S)(P,O,O(27,S))
#define BOOST_PP_WHILE26(P,O,S) BOOST_PP_WHILE_C(P(28,S),27,S)(P,O,O(28,S))
#define BOOST_PP_WHILE27(P,O,S) BOOST_PP_WHILE_C(P(29,S),28,S)(P,O,O(29,S))
#define BOOST_PP_WHILE28(P,O,S) BOOST_PP_WHILE_C(P(30,S),29,S)(P,O,O(30,S))
#define BOOST_PP_WHILE29(P,O,S) BOOST_PP_WHILE_C(P(31,S),30,S)(P,O,O(31,S))
#define BOOST_PP_WHILE30(P,O,S) BOOST_PP_WHILE_C(P(32,S),31,S)(P,O,O(32,S))
#define BOOST_PP_WHILE31(P,O,S) BOOST_PP_WHILE_C(P(33,S),32,S)(P,O,O(33,S))
#define BOOST_PP_WHILE32(P,O,S) BOOST_PP_WHILE_C(P(34,S),33,S)(P,O,O(34,S))
#define BOOST_PP_WHILE33(P,O,S) BOOST_PP_WHILE_C(P(35,S),34,S)(P,O,O(35,S))
#define BOOST_PP_WHILE34(P,O,S) BOOST_PP_WHILE_C(P(36,S),35,S)(P,O,O(36,S))
#define BOOST_PP_WHILE35(P,O,S) BOOST_PP_WHILE_C(P(37,S),36,S)(P,O,O(37,S))
#define BOOST_PP_WHILE36(P,O,S) BOOST_PP_WHILE_C(P(38,S),37,S)(P,O,O(38,S))
#define BOOST_PP_WHILE37(P,O,S) BOOST_PP_WHILE_C(P(39,S),38,S)(P,O,O(39,S))
#define BOOST_PP_WHILE38(P,O,S) BOOST_PP_WHILE_C(P(40,S),39,S)(P,O,O(40,S))
#define BOOST_PP_WHILE39(P,O,S) BOOST_PP_WHILE_C(P(41,S),40,S)(P,O,O(41,S))
#define BOOST_PP_WHILE40(P,O,S) BOOST_PP_WHILE_C(P(42,S),41,S)(P,O,O(42,S))
#define BOOST_PP_WHILE41(P,O,S) BOOST_PP_WHILE_C(P(43,S),42,S)(P,O,O(43,S))
#define BOOST_PP_WHILE42(P,O,S) BOOST_PP_WHILE_C(P(44,S),43,S)(P,O,O(44,S))
#define BOOST_PP_WHILE43(P,O,S) BOOST_PP_WHILE_C(P(45,S),44,S)(P,O,O(45,S))
#define BOOST_PP_WHILE44(P,O,S) BOOST_PP_WHILE_C(P(46,S),45,S)(P,O,O(46,S))
#define BOOST_PP_WHILE45(P,O,S) BOOST_PP_WHILE_C(P(47,S),46,S)(P,O,O(47,S))
#define BOOST_PP_WHILE46(P,O,S) BOOST_PP_WHILE_C(P(48,S),47,S)(P,O,O(48,S))
#define BOOST_PP_WHILE47(P,O,S) BOOST_PP_WHILE_C(P(49,S),48,S)(P,O,O(49,S))
#define BOOST_PP_WHILE48(P,O,S) BOOST_PP_WHILE_C(P(50,S),49,S)(P,O,O(50,S))
#define BOOST_PP_WHILE49(P,O,S) BOOST_PP_WHILE_C(P(51,S),50,S)(P,O,O(51,S))
#define BOOST_PP_WHILE50(P,O,S) BOOST_PP_WHILE_C(P(52,S),51,S)(P,O,O(52,S))
#define BOOST_PP_WHILE51(P,O,S) BOOST_PP_WHILE_C(P(53,S),52,S)(P,O,O(53,S))
#define BOOST_PP_WHILE52(P,O,S) BOOST_PP_WHILE_C(P(54,S),53,S)(P,O,O(54,S))
#define BOOST_PP_WHILE53(P,O,S) BOOST_PP_WHILE_C(P(55,S),54,S)(P,O,O(55,S))
#define BOOST_PP_WHILE54(P,O,S) BOOST_PP_WHILE_C(P(56,S),55,S)(P,O,O(56,S))
#define BOOST_PP_WHILE55(P,O,S) BOOST_PP_WHILE_C(P(57,S),56,S)(P,O,O(57,S))
#define BOOST_PP_WHILE56(P,O,S) BOOST_PP_WHILE_C(P(58,S),57,S)(P,O,O(58,S))
#define BOOST_PP_WHILE57(P,O,S) BOOST_PP_WHILE_C(P(59,S),58,S)(P,O,O(59,S))
#define BOOST_PP_WHILE58(P,O,S) BOOST_PP_WHILE_C(P(60,S),59,S)(P,O,O(60,S))
#define BOOST_PP_WHILE59(P,O,S) BOOST_PP_WHILE_C(P(61,S),60,S)(P,O,O(61,S))
#define BOOST_PP_WHILE60(P,O,S) BOOST_PP_WHILE_C(P(62,S),61,S)(P,O,O(62,S))
#define BOOST_PP_WHILE61(P,O,S) BOOST_PP_WHILE_C(P(63,S),62,S)(P,O,O(63,S))
#define BOOST_PP_WHILE62(P,O,S) BOOST_PP_WHILE_C(P(64,S),63,S)(P,O,O(64,S))
#define BOOST_PP_WHILE63(P,O,S) BOOST_PP_WHILE_C(P(65,S),64,S)(P,O,O(65,S))
#define BOOST_PP_WHILE64(P,O,S) BOOST_PP_WHILE_C(P(66,S),65,S)(P,O,O(66,S))
#define BOOST_PP_WHILE65(P,O,S) BOOST_PP_WHILE_C(P(67,S),66,S)(P,O,O(67,S))
#define BOOST_PP_WHILE66(P,O,S) BOOST_PP_WHILE_C(P(68,S),67,S)(P,O,O(68,S))
#define BOOST_PP_WHILE67(P,O,S) BOOST_PP_WHILE_C(P(69,S),68,S)(P,O,O(69,S))
#define BOOST_PP_WHILE68(P,O,S) BOOST_PP_WHILE_C(P(70,S),69,S)(P,O,O(70,S))
#define BOOST_PP_WHILE69(P,O,S) BOOST_PP_WHILE_C(P(71,S),70,S)(P,O,O(71,S))
#define BOOST_PP_WHILE70(P,O,S) BOOST_PP_WHILE_C(P(72,S),71,S)(P,O,O(72,S))
#define BOOST_PP_WHILE71(P,O,S) BOOST_PP_WHILE_C(P(73,S),72,S)(P,O,O(73,S))
#define BOOST_PP_WHILE72(P,O,S) BOOST_PP_WHILE_C(P(74,S),73,S)(P,O,O(74,S))
#define BOOST_PP_WHILE73(P,O,S) BOOST_PP_WHILE_C(P(75,S),74,S)(P,O,O(75,S))
#define BOOST_PP_WHILE74(P,O,S) BOOST_PP_WHILE_C(P(76,S),75,S)(P,O,O(76,S))
#define BOOST_PP_WHILE75(P,O,S) BOOST_PP_WHILE_C(P(77,S),76,S)(P,O,O(77,S))
#define BOOST_PP_WHILE76(P,O,S) BOOST_PP_WHILE_C(P(78,S),77,S)(P,O,O(78,S))
#define BOOST_PP_WHILE77(P,O,S) BOOST_PP_WHILE_C(P(79,S),78,S)(P,O,O(79,S))
#define BOOST_PP_WHILE78(P,O,S) BOOST_PP_WHILE_C(P(80,S),79,S)(P,O,O(80,S))
#define BOOST_PP_WHILE79(P,O,S) BOOST_PP_WHILE_C(P(81,S),80,S)(P,O,O(81,S))
#define BOOST_PP_WHILE80(P,O,S) BOOST_PP_WHILE_C(P(82,S),81,S)(P,O,O(82,S))
#define BOOST_PP_WHILE81(P,O,S) BOOST_PP_WHILE_C(P(83,S),82,S)(P,O,O(83,S))
#define BOOST_PP_WHILE82(P,O,S) BOOST_PP_WHILE_C(P(84,S),83,S)(P,O,O(84,S))
#define BOOST_PP_WHILE83(P,O,S) BOOST_PP_WHILE_C(P(85,S),84,S)(P,O,O(85,S))
#define BOOST_PP_WHILE84(P,O,S) BOOST_PP_WHILE_C(P(86,S),85,S)(P,O,O(86,S))
#define BOOST_PP_WHILE85(P,O,S) BOOST_PP_WHILE_C(P(87,S),86,S)(P,O,O(87,S))
#define BOOST_PP_WHILE86(P,O,S) BOOST_PP_WHILE_C(P(88,S),87,S)(P,O,O(88,S))
#define BOOST_PP_WHILE87(P,O,S) BOOST_PP_WHILE_C(P(89,S),88,S)(P,O,O(89,S))
#define BOOST_PP_WHILE88(P,O,S) BOOST_PP_WHILE_C(P(90,S),89,S)(P,O,O(90,S))
#define BOOST_PP_WHILE89(P,O,S) BOOST_PP_WHILE_C(P(91,S),90,S)(P,O,O(91,S))
#define BOOST_PP_WHILE90(P,O,S) BOOST_PP_WHILE_C(P(92,S),91,S)(P,O,O(92,S))
#define BOOST_PP_WHILE91(P,O,S) BOOST_PP_WHILE_C(P(93,S),92,S)(P,O,O(93,S))
#define BOOST_PP_WHILE92(P,O,S) BOOST_PP_WHILE_C(P(94,S),93,S)(P,O,O(94,S))
#define BOOST_PP_WHILE93(P,O,S) BOOST_PP_WHILE_C(P(95,S),94,S)(P,O,O(95,S))
#define BOOST_PP_WHILE94(P,O,S) BOOST_PP_WHILE_C(P(96,S),95,S)(P,O,O(96,S))
#define BOOST_PP_WHILE95(P,O,S) BOOST_PP_WHILE_C(P(97,S),96,S)(P,O,O(97,S))
#define BOOST_PP_WHILE96(P,O,S) BOOST_PP_WHILE_C(P(98,S),97,S)(P,O,O(98,S))
#define BOOST_PP_WHILE97(P,O,S) BOOST_PP_WHILE_C(P(99,S),98,S)(P,O,O(99,S))
#define BOOST_PP_WHILE98(P,O,S) BOOST_PP_WHILE_C(P(100,S),99,S)(P,O,O(100,S))
#define BOOST_PP_WHILE99(P,O,S) BOOST_PP_WHILE_C(P(101,S),100,S)(P,O,O(101,S))
#define BOOST_PP_WHILE100(P,O,S) BOOST_PP_WHILE_C(P(102,S),101,S)(P,O,O(102,S))
#define BOOST_PP_WHILE101(P,O,S) BOOST_PP_WHILE_C(P(103,S),102,S)(P,O,O(103,S))
#define BOOST_PP_WHILE102(P,O,S) BOOST_PP_WHILE_C(P(104,S),103,S)(P,O,O(104,S))
#define BOOST_PP_WHILE103(P,O,S) BOOST_PP_WHILE_C(P(105,S),104,S)(P,O,O(105,S))
#define BOOST_PP_WHILE104(P,O,S) BOOST_PP_WHILE_C(P(106,S),105,S)(P,O,O(106,S))
#define BOOST_PP_WHILE105(P,O,S) BOOST_PP_WHILE_C(P(107,S),106,S)(P,O,O(107,S))
#define BOOST_PP_WHILE106(P,O,S) BOOST_PP_WHILE_C(P(108,S),107,S)(P,O,O(108,S))
#define BOOST_PP_WHILE107(P,O,S) BOOST_PP_WHILE_C(P(109,S),108,S)(P,O,O(109,S))
#define BOOST_PP_WHILE108(P,O,S) BOOST_PP_WHILE_C(P(110,S),109,S)(P,O,O(110,S))
#define BOOST_PP_WHILE109(P,O,S) BOOST_PP_WHILE_C(P(111,S),110,S)(P,O,O(111,S))
#define BOOST_PP_WHILE110(P,O,S) BOOST_PP_WHILE_C(P(112,S),111,S)(P,O,O(112,S))
#define BOOST_PP_WHILE111(P,O,S) BOOST_PP_WHILE_C(P(113,S),112,S)(P,O,O(113,S))
#define BOOST_PP_WHILE112(P,O,S) BOOST_PP_WHILE_C(P(114,S),113,S)(P,O,O(114,S))
#define BOOST_PP_WHILE113(P,O,S) BOOST_PP_WHILE_C(P(115,S),114,S)(P,O,O(115,S))
#define BOOST_PP_WHILE114(P,O,S) BOOST_PP_WHILE_C(P(116,S),115,S)(P,O,O(116,S))
#define BOOST_PP_WHILE115(P,O,S) BOOST_PP_WHILE_C(P(117,S),116,S)(P,O,O(117,S))
#define BOOST_PP_WHILE116(P,O,S) BOOST_PP_WHILE_C(P(118,S),117,S)(P,O,O(118,S))
#define BOOST_PP_WHILE117(P,O,S) BOOST_PP_WHILE_C(P(119,S),118,S)(P,O,O(119,S))
#define BOOST_PP_WHILE118(P,O,S) BOOST_PP_WHILE_C(P(120,S),119,S)(P,O,O(120,S))
#define BOOST_PP_WHILE119(P,O,S) BOOST_PP_WHILE_C(P(121,S),120,S)(P,O,O(121,S))
#define BOOST_PP_WHILE120(P,O,S) BOOST_PP_WHILE_C(P(122,S),121,S)(P,O,O(122,S))
#define BOOST_PP_WHILE121(P,O,S) BOOST_PP_WHILE_C(P(123,S),122,S)(P,O,O(123,S))
#define BOOST_PP_WHILE122(P,O,S) BOOST_PP_WHILE_C(P(124,S),123,S)(P,O,O(124,S))
#define BOOST_PP_WHILE123(P,O,S) BOOST_PP_WHILE_C(P(125,S),124,S)(P,O,O(125,S))
#define BOOST_PP_WHILE124(P,O,S) BOOST_PP_WHILE_C(P(126,S),125,S)(P,O,O(126,S))
#define BOOST_PP_WHILE125(P,O,S) BOOST_PP_WHILE_C(P(127,S),126,S)(P,O,O(127,S))
#define BOOST_PP_WHILE126(P,O,S) BOOST_PP_WHILE_C(P(128,S),127,S)(P,O,O(128,S))
#define BOOST_PP_WHILE127(P,O,S) BOOST_PP_WHILE_C(P(129,S),128,S)(P,O,O(129,S))
#define BOOST_PP_WHILE128(P,O,S) BOOST_PP_WHILE_C(P(130,S),129,S)(P,O,O(130,S))
#define BOOST_PP_WHILE129(P,O,S) BOOST_PP_WHILE_C(P(131,S),130,S)(P,O,O(131,S))
#define BOOST_PP_WHILE130(P,O,S) BOOST_PP_WHILE_C(P(132,S),131,S)(P,O,O(132,S))
#define BOOST_PP_WHILE131(P,O,S) BOOST_PP_WHILE_C(P(133,S),132,S)(P,O,O(133,S))
#define BOOST_PP_WHILE132(P,O,S) BOOST_PP_WHILE_C(P(134,S),133,S)(P,O,O(134,S))
#define BOOST_PP_WHILE133(P,O,S) BOOST_PP_WHILE_C(P(135,S),134,S)(P,O,O(135,S))
#define BOOST_PP_WHILE134(P,O,S) BOOST_PP_WHILE_C(P(136,S),135,S)(P,O,O(136,S))
#define BOOST_PP_WHILE135(P,O,S) BOOST_PP_WHILE_C(P(137,S),136,S)(P,O,O(137,S))
#define BOOST_PP_WHILE136(P,O,S) BOOST_PP_WHILE_C(P(138,S),137,S)(P,O,O(138,S))
#define BOOST_PP_WHILE137(P,O,S) BOOST_PP_WHILE_C(P(139,S),138,S)(P,O,O(139,S))
#define BOOST_PP_WHILE138(P,O,S) BOOST_PP_WHILE_C(P(140,S),139,S)(P,O,O(140,S))
#define BOOST_PP_WHILE139(P,O,S) BOOST_PP_WHILE_C(P(141,S),140,S)(P,O,O(141,S))
#define BOOST_PP_WHILE140(P,O,S) BOOST_PP_WHILE_C(P(142,S),141,S)(P,O,O(142,S))
#define BOOST_PP_WHILE141(P,O,S) BOOST_PP_WHILE_C(P(143,S),142,S)(P,O,O(143,S))
#define BOOST_PP_WHILE142(P,O,S) BOOST_PP_WHILE_C(P(144,S),143,S)(P,O,O(144,S))
#define BOOST_PP_WHILE143(P,O,S) BOOST_PP_WHILE_C(P(145,S),144,S)(P,O,O(145,S))
#define BOOST_PP_WHILE144(P,O,S) BOOST_PP_WHILE_C(P(146,S),145,S)(P,O,O(146,S))
#define BOOST_PP_WHILE145(P,O,S) BOOST_PP_WHILE_C(P(147,S),146,S)(P,O,O(147,S))
#define BOOST_PP_WHILE146(P,O,S) BOOST_PP_WHILE_C(P(148,S),147,S)(P,O,O(148,S))
#define BOOST_PP_WHILE147(P,O,S) BOOST_PP_WHILE_C(P(149,S),148,S)(P,O,O(149,S))
#define BOOST_PP_WHILE148(P,O,S) BOOST_PP_WHILE_C(P(150,S),149,S)(P,O,O(150,S))
#define BOOST_PP_WHILE149(P,O,S) BOOST_PP_WHILE_C(P(151,S),150,S)(P,O,O(151,S))
#define BOOST_PP_WHILE150(P,O,S) BOOST_PP_WHILE_C(P(152,S),151,S)(P,O,O(152,S))
#define BOOST_PP_WHILE151(P,O,S) BOOST_PP_WHILE_C(P(153,S),152,S)(P,O,O(153,S))
#define BOOST_PP_WHILE152(P,O,S) BOOST_PP_WHILE_C(P(154,S),153,S)(P,O,O(154,S))
#define BOOST_PP_WHILE153(P,O,S) BOOST_PP_WHILE_C(P(155,S),154,S)(P,O,O(155,S))
#define BOOST_PP_WHILE154(P,O,S) BOOST_PP_WHILE_C(P(156,S),155,S)(P,O,O(156,S))
#define BOOST_PP_WHILE155(P,O,S) BOOST_PP_WHILE_C(P(157,S),156,S)(P,O,O(157,S))
#define BOOST_PP_WHILE156(P,O,S) BOOST_PP_WHILE_C(P(158,S),157,S)(P,O,O(158,S))
#define BOOST_PP_WHILE157(P,O,S) BOOST_PP_WHILE_C(P(159,S),158,S)(P,O,O(159,S))
#define BOOST_PP_WHILE158(P,O,S) BOOST_PP_WHILE_C(P(160,S),159,S)(P,O,O(160,S))
#define BOOST_PP_WHILE159(P,O,S) BOOST_PP_WHILE_C(P(161,S),160,S)(P,O,O(161,S))
#define BOOST_PP_WHILE160(P,O,S) BOOST_PP_WHILE_C(P(162,S),161,S)(P,O,O(162,S))
#define BOOST_PP_WHILE161(P,O,S) BOOST_PP_WHILE_C(P(163,S),162,S)(P,O,O(163,S))
#define BOOST_PP_WHILE162(P,O,S) BOOST_PP_WHILE_C(P(164,S),163,S)(P,O,O(164,S))
#define BOOST_PP_WHILE163(P,O,S) BOOST_PP_WHILE_C(P(165,S),164,S)(P,O,O(165,S))
#define BOOST_PP_WHILE164(P,O,S) BOOST_PP_WHILE_C(P(166,S),165,S)(P,O,O(166,S))
#define BOOST_PP_WHILE165(P,O,S) BOOST_PP_WHILE_C(P(167,S),166,S)(P,O,O(167,S))
#define BOOST_PP_WHILE166(P,O,S) BOOST_PP_WHILE_C(P(168,S),167,S)(P,O,O(168,S))
#define BOOST_PP_WHILE167(P,O,S) BOOST_PP_WHILE_C(P(169,S),168,S)(P,O,O(169,S))
#define BOOST_PP_WHILE168(P,O,S) BOOST_PP_WHILE_C(P(170,S),169,S)(P,O,O(170,S))
#define BOOST_PP_WHILE169(P,O,S) BOOST_PP_WHILE_C(P(171,S),170,S)(P,O,O(171,S))
#define BOOST_PP_WHILE170(P,O,S) BOOST_PP_WHILE_C(P(172,S),171,S)(P,O,O(172,S))
#define BOOST_PP_WHILE171(P,O,S) BOOST_PP_WHILE_C(P(173,S),172,S)(P,O,O(173,S))
#define BOOST_PP_WHILE172(P,O,S) BOOST_PP_WHILE_C(P(174,S),173,S)(P,O,O(174,S))
#define BOOST_PP_WHILE173(P,O,S) BOOST_PP_WHILE_C(P(175,S),174,S)(P,O,O(175,S))
#define BOOST_PP_WHILE174(P,O,S) BOOST_PP_WHILE_C(P(176,S),175,S)(P,O,O(176,S))
#define BOOST_PP_WHILE175(P,O,S) BOOST_PP_WHILE_C(P(177,S),176,S)(P,O,O(177,S))
#define BOOST_PP_WHILE176(P,O,S) BOOST_PP_WHILE_C(P(178,S),177,S)(P,O,O(178,S))
#define BOOST_PP_WHILE177(P,O,S) BOOST_PP_WHILE_C(P(179,S),178,S)(P,O,O(179,S))
#define BOOST_PP_WHILE178(P,O,S) BOOST_PP_WHILE_C(P(180,S),179,S)(P,O,O(180,S))
#define BOOST_PP_WHILE179(P,O,S) BOOST_PP_WHILE_C(P(181,S),180,S)(P,O,O(181,S))
#define BOOST_PP_WHILE180(P,O,S) BOOST_PP_WHILE_C(P(182,S),181,S)(P,O,O(182,S))
#define BOOST_PP_WHILE181(P,O,S) BOOST_PP_WHILE_C(P(183,S),182,S)(P,O,O(183,S))
#define BOOST_PP_WHILE182(P,O,S) BOOST_PP_WHILE_C(P(184,S),183,S)(P,O,O(184,S))
#define BOOST_PP_WHILE183(P,O,S) BOOST_PP_WHILE_C(P(185,S),184,S)(P,O,O(185,S))
#define BOOST_PP_WHILE184(P,O,S) BOOST_PP_WHILE_C(P(186,S),185,S)(P,O,O(186,S))
#define BOOST_PP_WHILE185(P,O,S) BOOST_PP_WHILE_C(P(187,S),186,S)(P,O,O(187,S))
#define BOOST_PP_WHILE186(P,O,S) BOOST_PP_WHILE_C(P(188,S),187,S)(P,O,O(188,S))
#define BOOST_PP_WHILE187(P,O,S) BOOST_PP_WHILE_C(P(189,S),188,S)(P,O,O(189,S))
#define BOOST_PP_WHILE188(P,O,S) BOOST_PP_WHILE_C(P(190,S),189,S)(P,O,O(190,S))
#define BOOST_PP_WHILE189(P,O,S) BOOST_PP_WHILE_C(P(191,S),190,S)(P,O,O(191,S))
#define BOOST_PP_WHILE190(P,O,S) BOOST_PP_WHILE_C(P(192,S),191,S)(P,O,O(192,S))
#define BOOST_PP_WHILE191(P,O,S) BOOST_PP_WHILE_C(P(193,S),192,S)(P,O,O(193,S))
#define BOOST_PP_WHILE192(P,O,S) BOOST_PP_WHILE_C(P(194,S),193,S)(P,O,O(194,S))
#define BOOST_PP_WHILE193(P,O,S) BOOST_PP_WHILE_C(P(195,S),194,S)(P,O,O(195,S))
#define BOOST_PP_WHILE194(P,O,S) BOOST_PP_WHILE_C(P(196,S),195,S)(P,O,O(196,S))
#define BOOST_PP_WHILE195(P,O,S) BOOST_PP_WHILE_C(P(197,S),196,S)(P,O,O(197,S))
#define BOOST_PP_WHILE196(P,O,S) BOOST_PP_WHILE_C(P(198,S),197,S)(P,O,O(198,S))
#define BOOST_PP_WHILE197(P,O,S) BOOST_PP_WHILE_C(P(199,S),198,S)(P,O,O(199,S))
#define BOOST_PP_WHILE198(P,O,S) BOOST_PP_WHILE_C(P(200,S),199,S)(P,O,O(200,S))
#define BOOST_PP_WHILE199(P,O,S) BOOST_PP_WHILE_C(P(201,S),200,S)(P,O,O(201,S))
#define BOOST_PP_WHILE200(P,O,S) BOOST_PP_WHILE_C(P(202,S),201,S)(P,O,O(202,S))
#define BOOST_PP_WHILE201(P,O,S) BOOST_PP_WHILE_C(P(203,S),202,S)(P,O,O(203,S))
#define BOOST_PP_WHILE202(P,O,S) BOOST_PP_WHILE_C(P(204,S),203,S)(P,O,O(204,S))
#define BOOST_PP_WHILE203(P,O,S) BOOST_PP_WHILE_C(P(205,S),204,S)(P,O,O(205,S))
#define BOOST_PP_WHILE204(P,O,S) BOOST_PP_WHILE_C(P(206,S),205,S)(P,O,O(206,S))
#define BOOST_PP_WHILE205(P,O,S) BOOST_PP_WHILE_C(P(207,S),206,S)(P,O,O(207,S))
#define BOOST_PP_WHILE206(P,O,S) BOOST_PP_WHILE_C(P(208,S),207,S)(P,O,O(208,S))
#define BOOST_PP_WHILE207(P,O,S) BOOST_PP_WHILE_C(P(209,S),208,S)(P,O,O(209,S))
#define BOOST_PP_WHILE208(P,O,S) BOOST_PP_WHILE_C(P(210,S),209,S)(P,O,O(210,S))
#define BOOST_PP_WHILE209(P,O,S) BOOST_PP_WHILE_C(P(211,S),210,S)(P,O,O(211,S))
#define BOOST_PP_WHILE210(P,O,S) BOOST_PP_WHILE_C(P(212,S),211,S)(P,O,O(212,S))
#define BOOST_PP_WHILE211(P,O,S) BOOST_PP_WHILE_C(P(213,S),212,S)(P,O,O(213,S))
#define BOOST_PP_WHILE212(P,O,S) BOOST_PP_WHILE_C(P(214,S),213,S)(P,O,O(214,S))
#define BOOST_PP_WHILE213(P,O,S) BOOST_PP_WHILE_C(P(215,S),214,S)(P,O,O(215,S))
#define BOOST_PP_WHILE214(P,O,S) BOOST_PP_WHILE_C(P(216,S),215,S)(P,O,O(216,S))
#define BOOST_PP_WHILE215(P,O,S) BOOST_PP_WHILE_C(P(217,S),216,S)(P,O,O(217,S))
#define BOOST_PP_WHILE216(P,O,S) BOOST_PP_WHILE_C(P(218,S),217,S)(P,O,O(218,S))
#define BOOST_PP_WHILE217(P,O,S) BOOST_PP_WHILE_C(P(219,S),218,S)(P,O,O(219,S))
#define BOOST_PP_WHILE218(P,O,S) BOOST_PP_WHILE_C(P(220,S),219,S)(P,O,O(220,S))
#define BOOST_PP_WHILE219(P,O,S) BOOST_PP_WHILE_C(P(221,S),220,S)(P,O,O(221,S))
#define BOOST_PP_WHILE220(P,O,S) BOOST_PP_WHILE_C(P(222,S),221,S)(P,O,O(222,S))
#define BOOST_PP_WHILE221(P,O,S) BOOST_PP_WHILE_C(P(223,S),222,S)(P,O,O(223,S))
#define BOOST_PP_WHILE222(P,O,S) BOOST_PP_WHILE_C(P(224,S),223,S)(P,O,O(224,S))
#define BOOST_PP_WHILE223(P,O,S) BOOST_PP_WHILE_C(P(225,S),224,S)(P,O,O(225,S))
#define BOOST_PP_WHILE224(P,O,S) BOOST_PP_WHILE_C(P(226,S),225,S)(P,O,O(226,S))
#define BOOST_PP_WHILE225(P,O,S) BOOST_PP_WHILE_C(P(227,S),226,S)(P,O,O(227,S))
#define BOOST_PP_WHILE226(P,O,S) BOOST_PP_WHILE_C(P(228,S),227,S)(P,O,O(228,S))
#define BOOST_PP_WHILE227(P,O,S) BOOST_PP_WHILE_C(P(229,S),228,S)(P,O,O(229,S))
#define BOOST_PP_WHILE228(P,O,S) BOOST_PP_WHILE_C(P(230,S),229,S)(P,O,O(230,S))
#define BOOST_PP_WHILE229(P,O,S) BOOST_PP_WHILE_C(P(231,S),230,S)(P,O,O(231,S))
#define BOOST_PP_WHILE230(P,O,S) BOOST_PP_WHILE_C(P(232,S),231,S)(P,O,O(232,S))
#define BOOST_PP_WHILE231(P,O,S) BOOST_PP_WHILE_C(P(233,S),232,S)(P,O,O(233,S))
#define BOOST_PP_WHILE232(P,O,S) BOOST_PP_WHILE_C(P(234,S),233,S)(P,O,O(234,S))
#define BOOST_PP_WHILE233(P,O,S) BOOST_PP_WHILE_C(P(235,S),234,S)(P,O,O(235,S))
#define BOOST_PP_WHILE234(P,O,S) BOOST_PP_WHILE_C(P(236,S),235,S)(P,O,O(236,S))
#define BOOST_PP_WHILE235(P,O,S) BOOST_PP_WHILE_C(P(237,S),236,S)(P,O,O(237,S))
#define BOOST_PP_WHILE236(P,O,S) BOOST_PP_WHILE_C(P(238,S),237,S)(P,O,O(238,S))
#define BOOST_PP_WHILE237(P,O,S) BOOST_PP_WHILE_C(P(239,S),238,S)(P,O,O(239,S))
#define BOOST_PP_WHILE238(P,O,S) BOOST_PP_WHILE_C(P(240,S),239,S)(P,O,O(240,S))
#define BOOST_PP_WHILE239(P,O,S) BOOST_PP_WHILE_C(P(241,S),240,S)(P,O,O(241,S))
#define BOOST_PP_WHILE240(P,O,S) BOOST_PP_WHILE_C(P(242,S),241,S)(P,O,O(242,S))
#define BOOST_PP_WHILE241(P,O,S) BOOST_PP_WHILE_C(P(243,S),242,S)(P,O,O(243,S))
#define BOOST_PP_WHILE242(P,O,S) BOOST_PP_WHILE_C(P(244,S),243,S)(P,O,O(244,S))
#define BOOST_PP_WHILE243(P,O,S) BOOST_PP_WHILE_C(P(245,S),244,S)(P,O,O(245,S))
#define BOOST_PP_WHILE244(P,O,S) BOOST_PP_WHILE_C(P(246,S),245,S)(P,O,O(246,S))
#define BOOST_PP_WHILE245(P,O,S) BOOST_PP_WHILE_C(P(247,S),246,S)(P,O,O(247,S))
#define BOOST_PP_WHILE246(P,O,S) BOOST_PP_WHILE_C(P(248,S),247,S)(P,O,O(248,S))
#define BOOST_PP_WHILE247(P,O,S) BOOST_PP_WHILE_C(P(249,S),248,S)(P,O,O(249,S))
#define BOOST_PP_WHILE248(P,O,S) BOOST_PP_WHILE_C(P(250,S),249,S)(P,O,O(250,S))
#define BOOST_PP_WHILE249(P,O,S) BOOST_PP_WHILE_C(P(251,S),250,S)(P,O,O(251,S))
#define BOOST_PP_WHILE250(P,O,S) BOOST_PP_WHILE_C(P(252,S),251,S)(P,O,O(252,S))
#define BOOST_PP_WHILE251(P,O,S) BOOST_PP_WHILE_C(P(253,S),252,S)(P,O,O(253,S))
#define BOOST_PP_WHILE252(P,O,S) BOOST_PP_WHILE_C(P(254,S),253,S)(P,O,O(254,S))
#define BOOST_PP_WHILE253(P,O,S) BOOST_PP_WHILE_C(P(255,S),254,S)(P,O,O(255,S))
#define BOOST_PP_WHILE254(P,O,S) BOOST_PP_WHILE_C(P(256,S),255,S)(P,O,O(256,S))
#define BOOST_PP_WHILE255(P,O,S) BOOST_PP_WHILE_C(P(257,S),256,S)(P,O,O(257,S))
#define BOOST_PP_WHILE256(P,O,S) BOOST_PP_WHILE_C(P(258,S),257,S)(P,O,O(258,S))
#define BOOST_PP_WHILE257(P,O,S) BOOST_PP_WHILE_C(P(259,S),258,S)(P,O,O(259,S))
#define BOOST_PP_WHILE258(P,O,S) (RECURSION DEPTH EXCEEDED!)
#define BOOST_PP_WHILE259(P,O,S) (RECURSION DEPTH EXCEEDED!)
#endif
