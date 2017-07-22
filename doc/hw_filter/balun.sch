<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="7.2.0">
<drawing>
<settings>
<setting alwaysvectorfont="yes"/>
<setting verticaltext="up"/>
</settings>
<grid distance="0.1" unitdist="inch" unit="inch" style="lines" multiple="1" display="no" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers>
<layer number="1" name="Top" color="4" fill="1" visible="no" active="no"/>
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
<package name="6-SMD">
<smd name="P4" x="1.9939" y="2.80035" dx="1.27" dy="1.2065" layer="1"/>
<smd name="P5" x="0" y="2.80035" dx="1.27" dy="1.2065" layer="1"/>
<smd name="P6" x="-1.9939" y="2.80035" dx="1.27" dy="1.2065" layer="1"/>
<smd name="P3" x="1.9939" y="-2.80035" dx="1.27" dy="1.2065" layer="1"/>
<smd name="P1" x="-1.9939" y="-2.80035" dx="1.27" dy="1.2065" layer="1"/>
<smd name="P2" x="0" y="-2.80035" dx="1.27" dy="1.2065" layer="1"/>
<wire x1="-3.4544" y1="1.9939" x2="-3.4544" y2="-1.9939" width="0.127" layer="51"/>
<wire x1="-3.4544" y1="-1.9939" x2="3.4544" y2="-1.9939" width="0.127" layer="51"/>
<wire x1="3.4544" y1="-1.9939" x2="3.4544" y2="1.9939" width="0.127" layer="51"/>
<wire x1="3.4544" y1="1.9939" x2="-3.4544" y2="1.9939" width="0.127" layer="51"/>
<wire x1="-3.4544" y1="1.9939" x2="-3.4544" y2="-1.9939" width="0.254" layer="21"/>
<wire x1="3.4544" y1="-1.9939" x2="3.4544" y2="1.9939" width="0.254" layer="21"/>
<text x="3.81" y="2.54" size="1.27" layer="25" rot="SR270">&gt;NAME</text>
</package>
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
<package name=".25_SPACING">
<smd name="V-" x="-4.445" y="0" dx="6.35" dy="2.54" layer="1"/>
<smd name="V+" x="9.525" y="0" dx="6.35" dy="2.54" layer="1"/>
<wire x1="-7.62" y1="-1.905" x2="-0.635" y2="-1.905" width="0.127" layer="21"/>
<wire x1="-0.635" y1="1.905" x2="-7.62" y2="1.905" width="0.127" layer="21"/>
<wire x1="12.7" y1="-1.905" x2="5.715" y2="-1.905" width="0.127" layer="21"/>
<wire x1="5.715" y1="1.905" x2="12.7" y2="1.905" width="0.127" layer="21"/>
<wire x1="-0.635" y1="1.905" x2="-0.635" y2="1.27" width="0.127" layer="21"/>
<wire x1="-0.635" y1="-1.27" x2="-0.635" y2="-1.905" width="0.127" layer="21"/>
<wire x1="5.715" y1="-1.27" x2="5.715" y2="-1.905" width="0.127" layer="21"/>
<wire x1="5.715" y1="1.905" x2="5.715" y2="1.27" width="0.127" layer="21"/>
<text x="5.715" y="1.905" size="1.27" layer="21">V+</text>
<text x="-0.635" y="-1.905" size="1.27" layer="21" rot="SR180">V-</text>
</package>
</packages>
<symbols>
<symbol name="CX2147">
<wire x1="-53.34" y1="22.86" x2="2.54" y2="22.86" width="0.254" layer="94"/>
<wire x1="2.54" y1="-20.32" x2="-53.34" y2="-20.32" width="0.254" layer="94"/>
<wire x1="-53.34" y1="-20.32" x2="-53.34" y2="22.86" width="0.254" layer="94"/>
<pin name="P1" x="-58.42" y="17.78" length="middle"/>
<pin name="CENTER_TAP" x="-58.42" y="7.62" length="middle"/>
<pin name="P3" x="-58.42" y="-12.7" length="middle"/>
<pin name="P4" x="7.62" y="-10.16" length="middle" rot="R180"/>
<wire x1="2.54" y1="22.86" x2="2.54" y2="-20.32" width="0.254" layer="94"/>
<pin name="P6" x="7.62" y="17.78" length="middle" rot="R180"/>
<text x="-53.34" y="22.86" size="1.778" layer="95">&gt;NAME</text>
<text x="-53.34" y="-22.86" size="1.778" layer="96">&gt;VALUE</text>
<pin name="NC" x="7.62" y="2.54" length="middle" rot="R180"/>
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
<symbol name="DIPOLE_ANT">
<wire x1="0" y1="-2.54" x2="0" y2="5.08" width="0.254" layer="94"/>
<wire x1="0" y1="5.08" x2="-5.08" y2="5.08" width="0.254" layer="94"/>
<wire x1="2.54" y1="-2.54" x2="2.54" y2="5.08" width="0.254" layer="94"/>
<wire x1="2.54" y1="5.08" x2="7.62" y2="5.08" width="0.254" layer="94"/>
<pin name="V+" x="0" y="-2.54" length="middle" rot="R90"/>
<pin name="V-" x="2.54" y="-2.54" length="middle" rot="R90"/>
<text x="5.08" y="2.54" size="1.27" layer="95">&gt;NAME</text>
<text x="5.08" y="0" size="1.27" layer="96">&gt;VALUE</text>
</symbol>
</symbols>
<devicesets>
<deviceset name="CX2147">
<gates>
<gate name="G$1" symbol="CX2147" x="12.7" y="0"/>
</gates>
<devices>
<device name="CX2147" package="6-SMD">
<connects>
<connect gate="G$1" pin="CENTER_TAP" pad="P2"/>
<connect gate="G$1" pin="NC" pad="P5"/>
<connect gate="G$1" pin="P1" pad="P1"/>
<connect gate="G$1" pin="P3" pad="P3"/>
<connect gate="G$1" pin="P4" pad="P4"/>
<connect gate="G$1" pin="P6" pad="P6"/>
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
<deviceset name=".25_CENTER_DIPOLE">
<gates>
<gate name="G$1" symbol="DIPOLE_ANT" x="0" y="2.54"/>
</gates>
<devices>
<device name=".250_CENTER" package=".25_SPACING">
<connects>
<connect gate="G$1" pin="V+" pad="V+"/>
<connect gate="G$1" pin="V-" pad="V-"/>
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
<part name="BALUN" library="balun" deviceset="CX2147" device="CX2147"/>
<part name="UNBAL_OUT" library="balun" deviceset="CONSMA013.062" device="END_MNT"/>
<part name="DIPOLE" library="balun" deviceset=".25_CENTER_DIPOLE" device=".250_CENTER"/>
</parts>
<sheets>
<sheet>
<plain>
</plain>
<instances>
<instance part="BALUN" gate="G$1" x="101.6" y="60.96"/>
<instance part="UNBAL_OUT" gate="G$1" x="58.42" y="101.6" rot="MR90"/>
<instance part="DIPOLE" gate="G$1" x="76.2" y="10.16" rot="R180"/>
</instances>
<busses>
</busses>
<nets>
<net name="UNBAL_SIG" class="0">
<segment>
<pinref part="BALUN" gate="G$1" pin="P6"/>
<wire x1="109.22" y1="78.74" x2="109.22" y2="96.52" width="0.1524" layer="91"/>
<pinref part="UNBAL_OUT" gate="G$1" pin="SIG"/>
<wire x1="109.22" y1="96.52" x2="66.04" y2="96.52" width="0.1524" layer="91"/>
<label x="101.6" y="99.06" size="1.778" layer="95"/>
</segment>
</net>
<net name="UNBAL_GND" class="0">
<segment>
<pinref part="UNBAL_OUT" gate="G$1" pin="GND"/>
<pinref part="BALUN" gate="G$1" pin="P1"/>
<wire x1="58.42" y1="96.52" x2="43.18" y2="96.52" width="0.1524" layer="91"/>
<wire x1="43.18" y1="96.52" x2="43.18" y2="78.74" width="0.1524" layer="91"/>
<label x="30.48" y="88.9" size="1.778" layer="95"/>
</segment>
</net>
<net name="BAL_GND" class="0">
<segment>
<pinref part="DIPOLE" gate="G$1" pin="V-"/>
<pinref part="BALUN" gate="G$1" pin="P3"/>
<wire x1="73.66" y1="12.7" x2="43.18" y2="12.7" width="0.1524" layer="91"/>
<wire x1="43.18" y1="12.7" x2="43.18" y2="48.26" width="0.1524" layer="91"/>
<label x="50.8" y="25.4" size="1.778" layer="95"/>
</segment>
</net>
<net name="BAL_SIG" class="0">
<segment>
<pinref part="BALUN" gate="G$1" pin="P4"/>
<pinref part="DIPOLE" gate="G$1" pin="V+"/>
<wire x1="109.22" y1="50.8" x2="109.22" y2="12.7" width="0.1524" layer="91"/>
<wire x1="109.22" y1="12.7" x2="76.2" y2="12.7" width="0.1524" layer="91"/>
<label x="114.3" y="25.4" size="1.778" layer="95"/>
</segment>
</net>
</nets>
</sheet>
</sheets>
</schematic>
</drawing>
</eagle>
