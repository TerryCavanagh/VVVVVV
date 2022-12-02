// =================================================================================================
//
//	Starling Framework
//	Copyright Gamua GmbH. All Rights Reserved.
//
//	This program is free software. You can redistribute and/or modify it
//	in accordance with the terms of the accompanying license agreement.
//
// =================================================================================================

package starling.text
{
    import flash.display.BitmapData;
    import flash.display3D.Context3DTextureFormat;
    import flash.geom.Rectangle;
    import flash.utils.ByteArray;

    import starling.textures.Texture;

    /** @private
     *  This class contains constants for the 'MINI' bitmap font. It's done that way to avoid
     *  a dependency on the 'mx.core' library (which is required for the 'Embed' statement).
     * 
     *  <p>The font is based on "uni05_53.ttf" from Craig Kroeger (http://www.miniml.com) and was
     *  converted to a Bitmap Font with "GlyphDesigner" from 71squared (http://www.71squared.com).
     *  </p> */
    internal class MiniBitmapFont
    {
        private static const BITMAP_WIDTH:int = 128;
        private static const BITMAP_HEIGHT:int = 64;
        private static const BITMAP_DATA:Array = [ 
            2027613533, 3413039936,  202148514, 2266925598, 4206886452, 4286853117,    2034947, 
            3202703399,  352977282, 2957757964, 3113652880, 2158068882, 1468709156, 2268063717, 
            2779310143, 2101025806, 3416509055, 4215794539, 3602168838, 1038056207, 1932393374, 
            3182285627, 3086802234, 1741291262, 2017257123, 3395280843,  984074419, 3049693147, 
            3986077023, 1055013549, 1806563255, 1754714962, 1577746187, 1124058786, 3888759258, 
            2482229043, 2916583666, 3743065328,  866060213, 1695195001, 2401582068, 3113347901, 
            2616521596, 1053798161, 2093370968, 4229025683,  560451479,  854767518, 2610241322, 
            4279041348, 4181572480, 4031244973,  587139110, 1081376765,  962217926,  783603325, 
            3605526425, 4102001916,  289204733, 2635140255, 3453981695, 3487854373, 2132197241, 
            3164775074, 4257640328,  770238970,  144664537,  707141570, 2934433071,  871272893, 
             512964596,  808491899,  481894297, 3095982481, 3598364156, 1710636358, 2904016319, 
            1751040139,  596966466, 1363963692,  465815609,  315567311, 4290666159, 4086022551, 
             179721458, 2221734970, 3942224988, 1519355876, 3292323782, 3933427230, 3314199893, 
            3736227348, 3846038425,  603088884, 2677349227, 3207069327, 3555275967, 3063054283, 
            3064577213, 3412044179,  693642210, 4280513949,  762928717, 1802215333, 3774849674, 
            4221155330,  970959395,  557220237, 2107226136, 3509822982, 3403284788, 4265820019, 
             898597576,  991077243, 2091615904, 3334716888,  633599866, 4218780109, 2216000376, 
             834870947, 2118009742, 1362731961,  236280636, 1274945142, 1458729366,  797960805, 
            3289369720, 2103717340, 3946406003, 2676522889, 1624104606, 1156993903, 3186170404, 
            2254499071, 1204911924, 1314218830, 3307086392, 2824275959, 3839865679, 2073394964, 
            1873329433, 1754205930, 1528429545, 1631106062, 2263272465, 4220497047, 3522893765, 
            3641376303,  707451487, 3452496787, 1390653868, 2620555793, 1027328684, 3419683476, 
            3662193703,  765701986, 3808279132,  786403271, 3824435837,  713234896, 4261856399, 
            3471930731, 3993492879, 1447960461, 1398434593, 1914230187, 2398643285, 4156374464, 
            3859339207, 3220700061, 3373248762, 3186030434, 1315917060, 2809852481, 4008553903, 
            4105611953, 1599499652, 3513857591,  877854499, 4198259455, 3648560077, 2838035419, 
            3255594190, 2465578457, 4263505201,  534904657, 2889261598, 1358214576, 1069250354, 
            3870010557, 2628896583, 3448610878,  442343309, 1024736866, 4015119133, 3250867279, 
            1513359261, 2442089596, 1944476762,  735490552,  426990058, 4234106111, 1204305707, 
            3330995265, 2398649368, 4221048123, 1724669255, 3801115709, 3489328790, 3896402933, 
            3696936939, 2836983295, 3656750393, 3349724512, 3810416287, 3654997608, 4284455103, 
            2294939563, 4207697932,  642748805, 2476981639, 2319419898,  572956615, 3833238940, 
             964924880, 2081600351, 3572458416, 2056247513, 1951368808, 2133449703, 2783728628, 
             512866577,  913279200, 1678129016, 3488578991, 3373952929, 2562996951, 3666058925, 
            1664169178, 1943591935,  750675303,  154399903, 2571590890,  852654952, 4117307766, 
            1971649621, 4180195820, 1222535348, 4283953215, 2880662236, 2717410980, 1175907705, 
            1157322027,  505963121, 2631540616, 3661227656, 3591803353, 2624126821, 1948662907, 
            3596065103, 1147387734,  256773959, 1173572460, 2361957471, 4210876076, 3080180620, 
            3464801210, 3821654259, 1465302035, 2851185457, 3143266144, 3793180414, 3368833103, 
            4274670712, 3473819108, 3487569332,  773123355, 1618635668, 2570176190, 2075248691, 
            1740805534,  288646743, 1837597401,  603556968, 3182536872,  673184603, 3088757053, 
            2897054404, 3192651316, 2885335802, 1057233368, 1118437241, 4182126463, 3110464775, 
            3313191614, 2360987274,  735505357, 2992631425, 2360928811, 4187834527,  279183208, 
            1586420003, 1174008423, 4062987589, 1162167621, 1162167621, 1162167621, 1162167621, 
            1174119799,  787274608
        ];
        
        private static const XML_DATA:XML = <font>
          <info face="mini" size="8" bold="0" italic="0" smooth="0"/>
          <common lineHeight="8" base="7" scaleW="128" scaleH="64" pages="1" packed="0"/>
          <chars count="191">
            <char id="195" x="1" y="1" width="5" height="9" xoffset="0" yoffset="-2" xadvance="6"/>
            <char id="209" x="7" y="1" width="5" height="9" xoffset="0" yoffset="-2" xadvance="6"/>
            <char id="213" x="13" y="1" width="5" height="9" xoffset="0" yoffset="-2" xadvance="6"/>
            <char id="253" x="19" y="1" width="4" height="9" xoffset="0" yoffset="0" xadvance="5"/>
            <char id="255" x="24" y="1" width="4" height="9" xoffset="0" yoffset="0" xadvance="5"/>
            <char id="192" x="29" y="1" width="5" height="8" xoffset="0" yoffset="-1" xadvance="6"/>
            <char id="193" x="35" y="1" width="5" height="8" xoffset="0" yoffset="-1" xadvance="6"/>
            <char id="194" x="41" y="1" width="5" height="8" xoffset="0" yoffset="-1" xadvance="6"/>
            <char id="197" x="47" y="1" width="5" height="8" xoffset="0" yoffset="-1" xadvance="6"/>
            <char id="200" x="53" y="1" width="5" height="8" xoffset="0" yoffset="-1" xadvance="6"/>
            <char id="201" x="59" y="1" width="5" height="8" xoffset="0" yoffset="-1" xadvance="6"/>
            <char id="202" x="65" y="1" width="5" height="8" xoffset="0" yoffset="-1" xadvance="6"/>
            <char id="210" x="71" y="1" width="5" height="8" xoffset="0" yoffset="-1" xadvance="6"/>
            <char id="211" x="77" y="1" width="5" height="8" xoffset="0" yoffset="-1" xadvance="6"/>
            <char id="212" x="83" y="1" width="5" height="8" xoffset="0" yoffset="-1" xadvance="6"/>
            <char id="217" x="89" y="1" width="5" height="8" xoffset="0" yoffset="-1" xadvance="6"/>
            <char id="218" x="95" y="1" width="5" height="8" xoffset="0" yoffset="-1" xadvance="6"/>
            <char id="219" x="101" y="1" width="5" height="8" xoffset="0" yoffset="-1" xadvance="6"/>
            <char id="221" x="107" y="1" width="5" height="8" xoffset="0" yoffset="-1" xadvance="6"/>
            <char id="206" x="113" y="1" width="3" height="8" xoffset="-1" yoffset="-1" xadvance="2"/>
            <char id="204" x="117" y="1" width="2" height="8" xoffset="-1" yoffset="-1" xadvance="2"/>
            <char id="205" x="120" y="1" width="2" height="8" xoffset="0" yoffset="-1" xadvance="2"/>
            <char id="36"  x="1" y="11" width="5" height="7" xoffset="0" yoffset="1" xadvance="6"/>
            <char id="196" x="7" y="11" width="5" height="7" xoffset="0" yoffset="0" xadvance="6"/>
            <char id="199" x="13" y="11" width="5" height="7" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="203" x="19" y="11" width="5" height="7" xoffset="0" yoffset="0" xadvance="6"/>
            <char id="214" x="25" y="11" width="5" height="7" xoffset="0" yoffset="0" xadvance="6"/>
            <char id="220" x="31" y="11" width="5" height="7" xoffset="0" yoffset="0" xadvance="6"/>
            <char id="224" x="37" y="11" width="4" height="7" xoffset="0" yoffset="0" xadvance="5"/>
            <char id="225" x="42" y="11" width="4" height="7" xoffset="0" yoffset="0" xadvance="5"/>
            <char id="226" x="47" y="11" width="4" height="7" xoffset="0" yoffset="0" xadvance="5"/>
            <char id="227" x="52" y="11" width="4" height="7" xoffset="0" yoffset="0" xadvance="5"/>
            <char id="232" x="57" y="11" width="4" height="7" xoffset="0" yoffset="0" xadvance="5"/>
            <char id="233" x="62" y="11" width="4" height="7" xoffset="0" yoffset="0" xadvance="5"/>
            <char id="234" x="67" y="11" width="4" height="7" xoffset="0" yoffset="0" xadvance="5"/>
            <char id="235" x="72" y="11" width="4" height="7" xoffset="0" yoffset="0" xadvance="5"/>
            <char id="241" x="77" y="11" width="4" height="7" xoffset="0" yoffset="0" xadvance="5"/>
            <char id="242" x="82" y="11" width="4" height="7" xoffset="0" yoffset="0" xadvance="5"/>
            <char id="243" x="87" y="11" width="4" height="7" xoffset="0" yoffset="0" xadvance="5"/>
            <char id="244" x="92" y="11" width="4" height="7" xoffset="0" yoffset="0" xadvance="5"/>
            <char id="245" x="97" y="11" width="4" height="7" xoffset="0" yoffset="0" xadvance="5"/>
            <char id="249" x="102" y="11" width="4" height="7" xoffset="0" yoffset="0" xadvance="5"/>
            <char id="250" x="107" y="11" width="4" height="7" xoffset="0" yoffset="0" xadvance="5"/>
            <char id="251" x="112" y="11" width="4" height="7" xoffset="0" yoffset="0" xadvance="5"/>
            <char id="254" x="117" y="11" width="4" height="7" xoffset="0" yoffset="2" xadvance="5"/>
            <char id="123" x="122" y="11" width="3" height="7" xoffset="0" yoffset="1" xadvance="4"/>
            <char id="125" x="1" y="19" width="3" height="7" xoffset="0" yoffset="1" xadvance="4"/>
            <char id="167" x="5" y="19" width="3" height="7" xoffset="0" yoffset="1" xadvance="4"/>
            <char id="207" x="9" y="19" width="3" height="7" xoffset="-1" yoffset="0" xadvance="2"/>
            <char id="106" x="13" y="19" width="2" height="7" xoffset="0" yoffset="2" xadvance="3"/>
            <char id="40" x="16" y="19" width="2" height="7" xoffset="0" yoffset="1" xadvance="3"/>
            <char id="41" x="19" y="19" width="2" height="7" xoffset="0" yoffset="1" xadvance="3"/>
            <char id="91" x="22" y="19" width="2" height="7" xoffset="0" yoffset="1" xadvance="3"/>
            <char id="93" x="25" y="19" width="2" height="7" xoffset="0" yoffset="1" xadvance="3"/>
            <char id="124" x="28" y="19" width="1" height="7" xoffset="1" yoffset="1" xadvance="4"/>
            <char id="81" x="30" y="19" width="5" height="6" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="163" x="36" y="19" width="5" height="6" xoffset="0" yoffset="1" xadvance="6"/>
            <char id="177" x="42" y="19" width="5" height="6" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="181" x="48" y="19" width="5" height="6" xoffset="0" yoffset="3" xadvance="6"/>
            <char id="103" x="54" y="19" width="4" height="6" xoffset="0" yoffset="3" xadvance="5"/>
            <char id="112" x="59" y="19" width="4" height="6" xoffset="0" yoffset="3" xadvance="5"/>
            <char id="113" x="64" y="19" width="4" height="6" xoffset="0" yoffset="3" xadvance="5"/>
            <char id="121" x="69" y="19" width="4" height="6" xoffset="0" yoffset="3" xadvance="5"/>
            <char id="162" x="74" y="19" width="4" height="6" xoffset="0" yoffset="2" xadvance="5"/>
            <char id="228" x="79" y="19" width="4" height="6" xoffset="0" yoffset="1" xadvance="5"/>
            <char id="229" x="84" y="19" width="4" height="6" xoffset="0" yoffset="1" xadvance="5"/>
            <char id="231" x="89" y="19" width="4" height="6" xoffset="0" yoffset="3" xadvance="5"/>
            <char id="240" x="94" y="19" width="4" height="6" xoffset="0" yoffset="1" xadvance="5"/>
            <char id="246" x="99" y="19" width="4" height="6" xoffset="0" yoffset="1" xadvance="5"/>
            <char id="252" x="104" y="19" width="4" height="6" xoffset="0" yoffset="1" xadvance="5"/>
            <char id="238" x="109" y="19" width="3" height="6" xoffset="-1" yoffset="1" xadvance="2"/>
            <char id="59" x="113" y="19" width="2" height="6" xoffset="0" yoffset="3" xadvance="4"/>
            <char id="236" x="116" y="19" width="2" height="6" xoffset="-1" yoffset="1" xadvance="2"/>
            <char id="237" x="119" y="19" width="2" height="6" xoffset="0" yoffset="1" xadvance="2"/>
            <char id="198" x="1" y="27" width="9" height="5" xoffset="0" yoffset="2" xadvance="10"/>
            <char id="190" x="11" y="27" width="8" height="5" xoffset="0" yoffset="2" xadvance="9"/>
            <char id="87" x="20" y="27" width="7" height="5" xoffset="0" yoffset="2" xadvance="8"/>
            <char id="188" x="28" y="27" width="7" height="5" xoffset="0" yoffset="2" xadvance="8"/>
            <char id="189" x="36" y="27" width="7" height="5" xoffset="0" yoffset="2" xadvance="8"/>
            <char id="38" x="44" y="27" width="6" height="5" xoffset="0" yoffset="2" xadvance="7"/>
            <char id="164" x="51" y="27" width="6" height="5" xoffset="0" yoffset="2" xadvance="7"/>
            <char id="208" x="58" y="27" width="6" height="5" xoffset="0" yoffset="2" xadvance="7"/>
            <char id="8364" x="65" y="27" width="6" height="5" xoffset="0" yoffset="2" xadvance="7"/>
            <char id="65" x="72" y="27" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="66" x="78" y="27" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="67" x="84" y="27" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="68" x="90" y="27" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="69" x="96" y="27" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="70" x="102" y="27" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="71" x="108" y="27" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="72" x="114" y="27" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="75" x="120" y="27" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="77" x="1" y="33" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="78" x="7" y="33" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="79" x="13" y="33" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="80" x="19" y="33" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="82" x="25" y="33" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="83" x="31" y="33" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="84" x="37" y="33" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="85" x="43" y="33" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="86" x="49" y="33" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="88" x="55" y="33" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="89" x="61" y="33" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="90" x="67" y="33" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="50" x="73" y="33" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="51" x="79" y="33" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="52" x="85" y="33" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="53" x="91" y="33" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="54" x="97" y="33" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="56" x="103" y="33" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="57" x="109" y="33" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="48" x="115" y="33" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="47" x="121" y="33" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="64" x="1" y="39" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="92" x="7" y="39" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="37" x="13" y="39" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="43" x="19" y="39" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="35" x="25" y="39" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="42" x="31" y="39" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="165" x="37" y="39" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="169" x="43" y="39" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="174" x="49" y="39" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="182" x="55" y="39" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="216" x="61" y="39" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="247" x="67" y="39" width="5" height="5" xoffset="0" yoffset="2" xadvance="6"/>
            <char id="74" x="73" y="39" width="4" height="5" xoffset="0" yoffset="2" xadvance="5"/>
            <char id="76" x="78" y="39" width="4" height="5" xoffset="0" yoffset="2" xadvance="5"/>
            <char id="98" x="83" y="39" width="4" height="5" xoffset="0" yoffset="2" xadvance="5"/>
            <char id="100" x="88" y="39" width="4" height="5" xoffset="0" yoffset="2" xadvance="5"/>
            <char id="104" x="93" y="39" width="4" height="5" xoffset="0" yoffset="2" xadvance="5"/>
            <char id="107" x="98" y="39" width="4" height="5" xoffset="0" yoffset="2" xadvance="5"/>
            <char id="55" x="103" y="39" width="4" height="5" xoffset="0" yoffset="2" xadvance="5"/>
            <char id="63" x="108" y="39" width="4" height="5" xoffset="0" yoffset="2" xadvance="5"/>
            <char id="191" x="113" y="39" width="4" height="5" xoffset="0" yoffset="2" xadvance="5"/>
            <char id="222" x="118" y="39" width="4" height="5" xoffset="0" yoffset="2" xadvance="5"/>
            <char id="223" x="123" y="39" width="4" height="5" xoffset="0" yoffset="2" xadvance="5"/>
            <char id="116" x="1" y="45" width="3" height="5" xoffset="0" yoffset="2" xadvance="4"/>
            <char id="60" x="5" y="45" width="3" height="5" xoffset="0" yoffset="2" xadvance="4"/>
            <char id="62" x="9" y="45" width="3" height="5" xoffset="0" yoffset="2" xadvance="4"/>
            <char id="170" x="13" y="45" width="3" height="5" xoffset="0" yoffset="2" xadvance="4"/>
            <char id="186" x="17" y="45" width="3" height="5" xoffset="0" yoffset="2" xadvance="4"/>
            <char id="239" x="21" y="45" width="3" height="5" xoffset="-1" yoffset="2" xadvance="2"/>
            <char id="102" x="25" y="45" width="2" height="5" xoffset="0" yoffset="2" xadvance="3"/>
            <char id="49" x="28" y="45" width="2" height="5" xoffset="0" yoffset="2" xadvance="3"/>
            <char id="73" x="31" y="45" width="1" height="5" xoffset="0" yoffset="2" xadvance="2"/>
            <char id="105" x="33" y="45" width="1" height="5" xoffset="0" yoffset="2" xadvance="2"/>
            <char id="108" x="35" y="45" width="1" height="5" xoffset="0" yoffset="2" xadvance="2"/>
            <char id="33" x="37" y="45" width="1" height="5" xoffset="1" yoffset="2" xadvance="3"/>
            <char id="161" x="39" y="45" width="1" height="5" xoffset="0" yoffset="2" xadvance="3"/>
            <char id="166" x="41" y="45" width="1" height="5" xoffset="0" yoffset="2" xadvance="2"/>
            <char id="109" x="43" y="45" width="7" height="4" xoffset="0" yoffset="3" xadvance="8"/>
            <char id="119" x="51" y="45" width="7" height="4" xoffset="0" yoffset="3" xadvance="8"/>
            <char id="230" x="59" y="45" width="7" height="4" xoffset="0" yoffset="3" xadvance="8"/>
            <char id="97" x="67" y="45" width="4" height="4" xoffset="0" yoffset="3" xadvance="5"/>
            <char id="99" x="72" y="45" width="4" height="4" xoffset="0" yoffset="3" xadvance="5"/>
            <char id="101" x="77" y="45" width="4" height="4" xoffset="0" yoffset="3" xadvance="5"/>
            <char id="110" x="82" y="45" width="4" height="4" xoffset="0" yoffset="3" xadvance="5"/>
            <char id="111" x="87" y="45" width="4" height="4" xoffset="0" yoffset="3" xadvance="5"/>
            <char id="115" x="92" y="45" width="4" height="4" xoffset="0" yoffset="3" xadvance="5"/>
            <char id="117" x="97" y="45" width="4" height="4" xoffset="0" yoffset="3" xadvance="5"/>
            <char id="118" x="102" y="45" width="4" height="4" xoffset="0" yoffset="3" xadvance="5"/>
            <char id="120" x="107" y="45" width="4" height="4" xoffset="0" yoffset="3" xadvance="5"/>
            <char id="122" x="112" y="45" width="4" height="4" xoffset="0" yoffset="3" xadvance="5"/>
            <char id="215" x="117" y="45" width="4" height="4" xoffset="0" yoffset="3" xadvance="5"/>
            <char id="248" x="122" y="45" width="4" height="4" xoffset="0" yoffset="3" xadvance="5"/>
            <char id="114" x="1" y="51" width="3" height="4" xoffset="0" yoffset="3" xadvance="4"/>
            <char id="178" x="5" y="51" width="3" height="4" xoffset="0" yoffset="2" xadvance="4"/>
            <char id="179" x="9" y="51" width="3" height="4" xoffset="0" yoffset="2" xadvance="4"/>
            <char id="185" x="13" y="51" width="1" height="4" xoffset="0" yoffset="2" xadvance="2"/>
            <char id="61" x="15" y="51" width="5" height="3" xoffset="0" yoffset="3" xadvance="6"/>
            <char id="171" x="21" y="51" width="5" height="3" xoffset="0" yoffset="3" xadvance="6"/>
            <char id="172" x="27" y="51" width="5" height="3" xoffset="0" yoffset="4" xadvance="6"/>
            <char id="187" x="33" y="51" width="5" height="3" xoffset="0" yoffset="3" xadvance="6"/>
            <char id="176" x="39" y="51" width="3" height="3" xoffset="0" yoffset="2" xadvance="4"/>
            <char id="44" x="43" y="51" width="2" height="3" xoffset="0" yoffset="6" xadvance="3"/>
            <char id="58" x="46" y="51" width="1" height="3" xoffset="1" yoffset="3" xadvance="4"/>
            <char id="94" x="48" y="51" width="4" height="2" xoffset="-1" yoffset="2" xadvance="4"/>
            <char id="126" x="53" y="51" width="4" height="2" xoffset="0" yoffset="3" xadvance="5"/>
            <char id="34" x="58" y="51" width="3" height="2" xoffset="0" yoffset="2" xadvance="4"/>
            <char id="96" x="62" y="51" width="2" height="2" xoffset="0" yoffset="2" xadvance="3"/>
            <char id="180" x="65" y="51" width="2" height="2" xoffset="0" yoffset="2" xadvance="3"/>
            <char id="184" x="68" y="51" width="2" height="2" xoffset="0" yoffset="7" xadvance="3"/>
            <char id="39" x="71" y="51" width="1" height="2" xoffset="0" yoffset="2" xadvance="2"/>
            <char id="95" x="73" y="51" width="5" height="1" xoffset="0" yoffset="7" xadvance="6"/>
            <char id="45" x="79" y="51" width="4" height="1" xoffset="0" yoffset="4" xadvance="5"/>
            <char id="173" x="84" y="51" width="4" height="1" xoffset="0" yoffset="4" xadvance="5"/>
            <char id="168" x="89" y="51" width="3" height="1" xoffset="1" yoffset="2" xadvance="5"/>
            <char id="175" x="93" y="51" width="3" height="1" xoffset="0" yoffset="2" xadvance="4"/>
            <char id="46" x="97" y="51" width="1" height="1" xoffset="0" yoffset="6" xadvance="2"/>
            <char id="183" x="99" y="51" width="1" height="1" xoffset="0" yoffset="4" xadvance="2"/>
            <char id="32" x="6" y="56" width="0" height="0" xoffset="0" yoffset="127" xadvance="3"/>
          </chars>
        </font>;
        
        public static function get texture():Texture
        {
            var bitmapData:BitmapData = getBitmapData();
            var format:String = Context3DTextureFormat.BGRA_PACKED;
            var texture:Texture = Texture.fromBitmapData(bitmapData, false, false, 1, format);
            bitmapData.dispose();
            bitmapData = null;

            texture.root.onRestore = function():void
            {
                bitmapData = getBitmapData();
                texture.root.uploadBitmapData(bitmapData);
                bitmapData.dispose();
                bitmapData = null;
            };

            return texture;
        }

        private static function getBitmapData():BitmapData
        {
            var bmpData:BitmapData = new BitmapData(BITMAP_WIDTH, BITMAP_HEIGHT);
            var bmpBytes:ByteArray = new ByteArray();
            var numBytes:int = BITMAP_DATA.length;
            
            for (var i:int=0; i<numBytes; ++i)
                bmpBytes.writeUnsignedInt(BITMAP_DATA[i]);
            
            bmpBytes.uncompress();
            bmpData.setPixels(new Rectangle(0, 0, BITMAP_WIDTH, BITMAP_HEIGHT), bmpBytes);
            bmpBytes.clear();
            
            return bmpData;
        }
        
        public static function get xml():XML { return XML_DATA; }
    }
}