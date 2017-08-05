<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="8.0.0">
<drawing>
<settings>
<setting alwaysvectorfont="yes"/>
<setting verticaltext="up"/>
</settings>
<grid distance="0.1" unitdist="inch" unit="inch" style="lines" multiple="1" display="no" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers>
<layer number="1" name="Top" color="4" fill="1" visible="no" active="no"/>
<layer number="2" name="Route2" color="1" fill="3" visible="no" active="no"/>
<layer number="3" name="Route3" color="4" fill="3" visible="no" active="no"/>
<layer number="4" name="Route4" color="1" fill="4" visible="no" active="no"/>
<layer number="5" name="Route5" color="4" fill="4" visible="no" active="no"/>
<layer number="6" name="Route6" color="1" fill="8" visible="no" active="no"/>
<layer number="7" name="Route7" color="4" fill="8" visible="no" active="no"/>
<layer number="8" name="Route8" color="1" fill="2" visible="no" active="no"/>
<layer number="9" name="Route9" color="4" fill="2" visible="no" active="no"/>
<layer number="10" name="Route10" color="1" fill="7" visible="no" active="no"/>
<layer number="11" name="Route11" color="4" fill="7" visible="no" active="no"/>
<layer number="12" name="Route12" color="1" fill="5" visible="no" active="no"/>
<layer number="13" name="Route13" color="4" fill="5" visible="no" active="no"/>
<layer number="14" name="Route14" color="1" fill="6" visible="no" active="no"/>
<layer number="15" name="Route15" color="4" fill="6" visible="no" active="no"/>
<layer number="16" name="Bottom" color="1" fill="1" visible="no" active="no"/>
<layer number="17" name="Pads" color="2" fill="1" visible="no" active="no"/>
<layer number="18" name="Vias" color="2" fill="1" visible="no" active="no"/>
<layer number="19" name="Unrouted" color="6" fill="1" visible="no" active="no"/>
<layer number="20" name="Dimension" color="15" fill="1" visible="no" active="no"/>
<layer number="21" name="tPlace" color="23" fill="1" visible="no" active="no"/>
<layer number="22" name="bPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="23" name="tOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="24" name="bOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="25" name="tNames" color="7" fill="1" visible="no" active="no"/>
<layer number="26" name="bNames" color="7" fill="1" visible="no" active="no"/>
<layer number="27" name="tValues" color="7" fill="1" visible="no" active="no"/>
<layer number="28" name="bValues" color="7" fill="1" visible="no" active="no"/>
<layer number="29" name="tStop" color="7" fill="3" visible="no" active="no"/>
<layer number="30" name="bStop" color="7" fill="6" visible="no" active="no"/>
<layer number="31" name="tCream" color="7" fill="4" visible="no" active="no"/>
<layer number="32" name="bCream" color="7" fill="5" visible="no" active="no"/>
<layer number="33" name="tFinish" color="6" fill="3" visible="no" active="no"/>
<layer number="34" name="bFinish" color="6" fill="6" visible="no" active="no"/>
<layer number="35" name="tGlue" color="7" fill="4" visible="no" active="no"/>
<layer number="36" name="bGlue" color="7" fill="5" visible="no" active="no"/>
<layer number="37" name="tTest" color="7" fill="1" visible="no" active="no"/>
<layer number="38" name="bTest" color="7" fill="1" visible="no" active="no"/>
<layer number="39" name="tKeepout" color="4" fill="11" visible="no" active="no"/>
<layer number="40" name="bKeepout" color="1" fill="11" visible="no" active="no"/>
<layer number="41" name="tRestrict" color="4" fill="10" visible="no" active="no"/>
<layer number="42" name="bRestrict" color="1" fill="10" visible="no" active="no"/>
<layer number="43" name="vRestrict" color="2" fill="10" visible="no" active="no"/>
<layer number="44" name="Drills" color="7" fill="1" visible="no" active="no"/>
<layer number="45" name="Holes" color="7" fill="1" visible="no" active="no"/>
<layer number="46" name="Milling" color="3" fill="1" visible="no" active="no"/>
<layer number="47" name="Measures" color="7" fill="1" visible="no" active="no"/>
<layer number="48" name="Document" color="7" fill="1" visible="no" active="no"/>
<layer number="49" name="Reference" color="7" fill="1" visible="no" active="no"/>
<layer number="51" name="tDocu" color="6" fill="1" visible="no" active="no"/>
<layer number="52" name="bDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="90" name="Modules" color="5" fill="1" visible="yes" active="yes"/>
<layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes"/>
<layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes"/>
<layer number="93" name="Pins" color="2" fill="1" visible="no" active="yes"/>
<layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes"/>
<layer number="95" name="Names" color="7" fill="1" visible="yes" active="yes"/>
<layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes"/>
<layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes"/>
<layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries>
<library name="balun">
<packages>
<package name="SMA_EDGE_MNT">
<smd name="GND1" x="-2.54" y="0" dx="1.524" dy="5.08" layer="16"/>
<smd name="SIG" x="0" y="0" dx="1.524" dy="5.08" layer="1"/>
<smd name="GND2" x="2.54" y="0" dx="1.524" dy="5.08" layer="16"/>
<wire x1="-3.175" y1="2.54" x2="-3.175" y2="-2.2098" width="0.127" layer="51"/>
<wire x1="-3.175" y1="-2.2098" x2="3.175" y2="-2.2098" width="0.127" layer="51"/>
<wire x1="3.175" y1="-2.2098" x2="3.175" y2="2.54" width="0.127" layer="51"/>
<wire x1="3.175" y1="2.54" x2="2.54" y2="2.54" width="0.127" layer="51"/>
<wire x1="2.54" y1="2.54" x2="-2.54" y2="2.54" width="0.127" layer="51"/>
<wire x1="-2.54" y1="2.54" x2="-3.175" y2="2.54" width="0.127" layer="51"/>
<wire x1="-3.81" y1="2.54" x2="-3.81" y2="-3.81" width="0.127" layer="21"/>
<wire x1="3.81" y1="-3.81" x2="3.81" y2="2.54" width="0.127" layer="21"/>
<smd name="GND3" x="-2.54" y="0" dx="1.524" dy="5.08" layer="1"/>
<smd name="GND4" x="2.54" y="0" dx="1.524" dy="5.08" layer="1"/>
<wire x1="-2.54" y1="2.54" x2="-2.54" y2="5.08" width="0.127" layer="51"/>
<wire x1="-2.54" y1="5.08" x2="2.54" y2="5.08" width="0.127" layer="51"/>
<wire x1="2.54" y1="5.08" x2="2.54" y2="2.54" width="0.127" layer="51"/>
<text x="5.08" y="2.54" size="1.27" layer="25" rot="SR270">&gt;NAME</text>
</package>
<package name="HF1139">
<smd name="RF_IN" x="-7.62" y="-5.2705" dx="1.524" dy="1.397" layer="1"/>
<smd name="GND3" x="-2.54" y="-5.2705" dx="1.524" dy="1.397" layer="1"/>
<smd name="GND5" x="2.54" y="-5.2705" dx="1.524" dy="1.397" layer="1"/>
<smd name="GND7" x="7.62" y="-5.2705" dx="1.524" dy="1.397" layer="1"/>
<smd name="GND2" x="-7.62" y="5.2705" dx="1.524" dy="1.397" layer="1"/>
<smd name="GND4" x="-2.54" y="5.2705" dx="1.524" dy="1.397" layer="1"/>
<smd name="GND6" x="2.54" y="5.2705" dx="1.524" dy="1.397" layer="1"/>
<smd name="RF_OUT" x="7.62" y="5.2705" dx="1.524" dy="1.397" layer="1"/>
<wire x1="-9.398" y1="-5.588" x2="-9.398" y2="5.588" width="0.127" layer="21"/>
<wire x1="-6.35" y1="-5.588" x2="-3.81" y2="-5.588" width="0.127" layer="21"/>
<wire x1="-1.27" y1="-5.588" x2="1.27" y2="-5.588" width="0.127" layer="21"/>
<wire x1="3.81" y1="-5.588" x2="6.35" y2="-5.588" width="0.127" layer="21"/>
<wire x1="-6.35" y1="5.588" x2="-3.81" y2="5.588" width="0.127" layer="21"/>
<wire x1="-1.27" y1="5.588" x2="1.27" y2="5.588" width="0.127" layer="21"/>
<wire x1="3.81" y1="5.588" x2="6.35" y2="5.588" width="0.127" layer="21"/>
<wire x1="9.398" y1="5.588" x2="9.398" y2="-5.588" width="0.127" layer="21"/>
<text x="-10.16" y="-5.08" size="1.27" layer="21" rot="R90">&gt;NAME</text>
</package>
</packages>
<symbols>
<symbol name="CONSMA013.062_JACK">
<wire x1="-2.54" y1="-2.54" x2="0" y2="0" width="0.254" layer="94"/>
<wire x1="-2.54" y1="2.54" x2="0" y2="0" width="0.254" layer="94"/>
<wire x1="0" y1="0" x2="10.16" y2="0" width="0.254" layer="94"/>
<wire x1="-2.54" y1="5.08" x2="0" y2="7.62" width="0.254" layer="94"/>
<wire x1="-2.54" y1="10.16" x2="0" y2="7.62" width="0.254" layer="94"/>
<wire x1="10.16" y1="0" x2="10.16" y2="5.08" width="0.254" layer="94"/>
<wire x1="10.16" y1="5.08" x2="12.7" y2="5.08" width="0.254" layer="94"/>
<wire x1="10.16" y1="5.08" x2="7.62" y2="5.08" width="0.254" layer="94"/>
<circle x="10.16" y="7.62" radius="2.54" width="0.254" layer="94"/>
<wire x1="0" y1="7.62" x2="20.32" y2="7.62" width="0.254" layer="94"/>
<wire x1="10.16" y1="0" x2="20.32" y2="0" width="0.254" layer="94"/>
<pin name="SIG" x="25.4" y="7.62" length="middle" rot="R180"/>
<pin name="GND" x="25.4" y="0" length="middle" rot="R180"/>
<text x="0" y="-5.08" size="1.778" layer="95">&gt;NAME</text>
<text x="0" y="10.16" size="1.778" layer="96">&gt;VALUE</text>
</symbol>
<symbol name="CONSMA013.062">
<wire x1="0" y1="0" x2="10.16" y2="0" width="0.254" layer="94"/>
<wire x1="10.16" y1="0" x2="20.32" y2="0" width="0.254" layer="94"/>
<wire x1="10.16" y1="0" x2="10.16" y2="5.08" width="0.254" layer="94"/>
<wire x1="10.16" y1="5.08" x2="7.62" y2="5.08" width="0.254" layer="94"/>
<wire x1="10.16" y1="5.08" x2="12.7" y2="5.08" width="0.254" layer="94"/>
<circle x="10.16" y="7.62" radius="2.54" width="0.254" layer="94"/>
<wire x1="0" y1="7.62" x2="20.32" y2="7.62" width="0.254" layer="94"/>
<wire x1="20.32" y1="7.62" x2="17.78" y2="10.16" width="0.254" layer="94"/>
<wire x1="20.32" y1="7.62" x2="17.78" y2="5.08" width="0.254" layer="94"/>
<wire x1="20.32" y1="0" x2="17.78" y2="2.54" width="0.254" layer="94"/>
<wire x1="20.32" y1="0" x2="17.78" y2="-2.54" width="0.254" layer="94"/>
<pin name="SIG" x="-5.08" y="7.62" length="middle"/>
<pin name="GND" x="-5.08" y="0" length="middle"/>
<text x="-5.08" y="-5.08" size="1.778" layer="95">&gt;NAME</text>
<text x="-5.08" y="10.16" size="1.778" layer="96">&gt;VALUE</text>
</symbol>
<symbol name="SXBP-169+">
<pin name="RF_IN" x="-7.62" y="2.54" visible="pad" length="short" direction="in"/>
<pin name="GND_IN" x="-7.62" y="-2.54" visible="pad" length="short"/>
<pin name="RF_OUT" x="7.62" y="2.54" visible="pad" length="short" direction="out" rot="R180"/>
<wire x1="-5.08" y1="5.08" x2="5.08" y2="5.08" width="0.254" layer="94"/>
<wire x1="5.08" y1="5.08" x2="5.08" y2="-5.08" width="0.254" layer="94"/>
<wire x1="5.08" y1="-5.08" x2="-5.08" y2="-5.08" width="0.254" layer="94"/>
<wire x1="-5.08" y1="-5.08" x2="-5.08" y2="5.08" width="0.254" layer="94"/>
<wire x1="-3.81" y1="-1.778" x2="-3.556" y2="-1.778" width="0.1524" layer="94"/>
<wire x1="-3.556" y1="-1.778" x2="-1.778" y2="-0.508" width="0.1524" layer="94" curve="71.075356"/>
<wire x1="-1.778" y1="-0.508" x2="-1.27" y2="1.27" width="0.1524" layer="94" curve="5.958497"/>
<wire x1="-1.27" y1="1.27" x2="-0.762" y2="2.286" width="0.1524" layer="94" curve="-27.197759"/>
<wire x1="-0.762" y1="2.286" x2="0.508" y2="2.286" width="0.1524" layer="94" curve="-99.675484"/>
<wire x1="0.508" y1="2.286" x2="1.016" y2="1.27" width="0.1524" layer="94" curve="-27.194019"/>
<wire x1="1.016" y1="1.27" x2="1.524" y2="-0.508" width="0.1524" layer="94" curve="5.957035"/>
<wire x1="1.524" y1="-0.508" x2="3.556" y2="-1.778" width="0.1524" layer="94" curve="78.141167"/>
<text x="-5.08" y="6.35" size="1.778" layer="95">&gt;NAME</text>
<text x="-5.08" y="-7.62" size="1.778" layer="96">&gt;VALUE</text>
</symbol>
</symbols>
<devicesets>
<deviceset name="CONSMA013.062_JACK">
<gates>
<gate name="G$1" symbol="CONSMA013.062_JACK" x="-10.16" y="0"/>
</gates>
<devices>
<device name="END_MNT" package="SMA_EDGE_MNT">
<connects>
<connect gate="G$1" pin="GND" pad="GND1 GND2 GND3 GND4"/>
<connect gate="G$1" pin="SIG" pad="SIG"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="CONSMA013.062">
<gates>
<gate name="G$1" symbol="CONSMA013.062" x="-10.16" y="0"/>
</gates>
<devices>
<device name="END_MNT" package="SMA_EDGE_MNT">
<connects>
<connect gate="G$1" pin="GND" pad="GND1 GND2 GND3 GND4"/>
<connect gate="G$1" pin="SIG" pad="SIG"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="SXBP-169+">
<gates>
<gate name="SXBP-169+" symbol="SXBP-169+" x="0" y="0"/>
</gates>
<devices>
<device name="SXBP-169+" package="HF1139">
<connects>
<connect gate="SXBP-169+" pin="GND_IN" pad="GND2 GND3 GND4 GND5 GND6 GND7"/>
<connect gate="SXBP-169+" pin="RF_IN" pad="RF_IN"/>
<connect gate="SXBP-169+" pin="RF_OUT" pad="RF_OUT"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
</libraries>
<attributes>
</attributes>
<variantdefs>
</variantdefs>
<classes>
<class number="0" name="default" width="0" drill="0">
</class>
</classes>
<parts>
<part name="INPUT" library="balun" deviceset="CONSMA013.062_JACK" device="END_MNT"/>
<part name="OUTPUT" library="balun" deviceset="CONSMA013.062" device="END_MNT"/>
<part name="FILTER" library="balun" deviceset="SXBP-169+" device="SXBP-169+"/>
</parts>
<sheets>
<sheet>
<plain>
</plain>
<instances>
<instance part="INPUT" gate="G$1" x="-12.7" y="53.34"/>
<instance part="OUTPUT" gate="G$1" x="106.68" y="53.34"/>
<instance part="FILTER" gate="SXBP-169+" x="53.34" y="58.42"/>
</instances>
<busses>
</busses>
<nets>
<net name="N$1" class="0">
<segment>
<pinref part="INPUT" gate="G$1" pin="SIG"/>
<pinref part="FILTER" gate="SXBP-169+" pin="RF_IN"/>
<wire x1="12.7" y1="60.96" x2="45.72" y2="60.96" width="0.1524" layer="91"/>
</segment>
</net>
<net name="GND" class="0">
<segment>
<pinref part="INPUT" gate="G$1" pin="GND"/>
<pinref part="FILTER" gate="SXBP-169+" pin="GND_IN"/>
<wire x1="12.7" y1="53.34" x2="45.72" y2="53.34" width="0.1524" layer="91"/>
<wire x1="45.72" y1="53.34" x2="45.72" y2="55.88" width="0.1524" layer="91"/>
<wire x1="45.72" y1="55.88" x2="45.72" y2="48.26" width="0.1524" layer="91"/>
<wire x1="45.72" y1="48.26" x2="101.6" y2="48.26" width="0.1524" layer="91"/>
<junction x="45.72" y="55.88"/>
<pinref part="OUTPUT" gate="G$1" pin="GND"/>
<wire x1="101.6" y1="48.26" x2="101.6" y2="53.34" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$3" class="0">
<segment>
<pinref part="OUTPUT" gate="G$1" pin="SIG"/>
<pinref part="FILTER" gate="SXBP-169+" pin="RF_OUT"/>
<wire x1="101.6" y1="60.96" x2="60.96" y2="60.96" width="0.1524" layer="91"/>
</segment>
</net>
</nets>
</sheet>
</sheets>
</schematic>
</drawing>
</eagle>
