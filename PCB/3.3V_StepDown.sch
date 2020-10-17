<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="9.4.2">
<drawing>
<settings>
<setting alwaysvectorfont="no"/>
<setting verticaltext="up"/>
</settings>
<grid distance="0.1" unitdist="inch" unit="inch" style="lines" multiple="1" display="no" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers>
<layer number="1" name="Top" color="4" fill="1" visible="no" active="no"/>
<layer number="2" name="Route2" color="16" fill="1" visible="no" active="no"/>
<layer number="3" name="Route3" color="17" fill="1" visible="no" active="no"/>
<layer number="4" name="Route4" color="18" fill="1" visible="no" active="no"/>
<layer number="5" name="Route5" color="19" fill="1" visible="no" active="no"/>
<layer number="6" name="Route6" color="25" fill="1" visible="no" active="no"/>
<layer number="7" name="Route7" color="26" fill="1" visible="no" active="no"/>
<layer number="8" name="Route8" color="27" fill="1" visible="no" active="no"/>
<layer number="9" name="Route9" color="28" fill="1" visible="no" active="no"/>
<layer number="10" name="Route10" color="29" fill="1" visible="no" active="no"/>
<layer number="11" name="Route11" color="30" fill="1" visible="no" active="no"/>
<layer number="12" name="Route12" color="20" fill="1" visible="no" active="no"/>
<layer number="13" name="Route13" color="21" fill="1" visible="no" active="no"/>
<layer number="14" name="Route14" color="22" fill="1" visible="no" active="no"/>
<layer number="15" name="Route15" color="23" fill="1" visible="no" active="no"/>
<layer number="16" name="Bottom" color="1" fill="1" visible="no" active="no"/>
<layer number="17" name="Pads" color="2" fill="1" visible="no" active="no"/>
<layer number="18" name="Vias" color="2" fill="1" visible="no" active="no"/>
<layer number="19" name="Unrouted" color="6" fill="1" visible="no" active="no"/>
<layer number="20" name="Dimension" color="24" fill="1" visible="no" active="no"/>
<layer number="21" name="tPlace" color="7" fill="1" visible="no" active="no"/>
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
<layer number="51" name="tDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="52" name="bDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="88" name="SimResults" color="9" fill="1" visible="yes" active="yes"/>
<layer number="89" name="SimProbes" color="9" fill="1" visible="yes" active="yes"/>
<layer number="90" name="Modules" color="5" fill="1" visible="yes" active="yes"/>
<layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes"/>
<layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes"/>
<layer number="93" name="Pins" color="2" fill="1" visible="no" active="yes"/>
<layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes"/>
<layer number="95" name="Names" color="7" fill="1" visible="yes" active="yes"/>
<layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes"/>
<layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes"/>
<layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes"/>
<layer number="99" name="SpiceOrder" color="5" fill="1" visible="yes" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries>
<library name="MouserUseful">
<description>&lt;Switching Voltage Regulators 3V-17V 1A Step-Down Converter&lt;/b&gt;&lt;p&gt;
&lt;author&gt;Created by SamacSys&lt;/author&gt;</description>
<packages>
<package name="SOP65P490X110-8N" urn="urn:adsk.eagle:footprint:13422154/1">
<description>8-SOP, 0.65 mm pitch, 4.90 mm span, 3.00 X 3.00 X 1.10 mm body
&lt;p&gt;8-pin SOP package with 0.65 mm pitch, 4.90 mm span with body size 3.00 X 3.00 X 1.10 mm&lt;/p&gt;</description>
<circle x="-2.2175" y="1.7197" radius="0.25" width="0" layer="21"/>
<wire x1="-1.55" y1="1.4697" x2="-1.55" y2="1.55" width="0.12" layer="21"/>
<wire x1="-1.55" y1="1.55" x2="1.55" y2="1.55" width="0.12" layer="21"/>
<wire x1="1.55" y1="1.55" x2="1.55" y2="1.4697" width="0.12" layer="21"/>
<wire x1="-1.55" y1="-1.4697" x2="-1.55" y2="-1.55" width="0.12" layer="21"/>
<wire x1="-1.55" y1="-1.55" x2="1.55" y2="-1.55" width="0.12" layer="21"/>
<wire x1="1.55" y1="-1.55" x2="1.55" y2="-1.4697" width="0.12" layer="21"/>
<wire x1="1.55" y1="-1.55" x2="-1.55" y2="-1.55" width="0.12" layer="51"/>
<wire x1="-1.55" y1="-1.55" x2="-1.55" y2="1.55" width="0.12" layer="51"/>
<wire x1="-1.55" y1="1.55" x2="1.55" y2="1.55" width="0.12" layer="51"/>
<wire x1="1.55" y1="1.55" x2="1.55" y2="-1.55" width="0.12" layer="51"/>
<smd name="1" x="-2.1496" y="0.975" dx="1.4709" dy="0.4815" layer="1"/>
<smd name="2" x="-2.1496" y="0.325" dx="1.4709" dy="0.4815" layer="1"/>
<smd name="3" x="-2.1496" y="-0.325" dx="1.4709" dy="0.4815" layer="1"/>
<smd name="4" x="-2.1496" y="-0.975" dx="1.4709" dy="0.4815" layer="1"/>
<smd name="5" x="2.1496" y="-0.975" dx="1.4709" dy="0.4815" layer="1"/>
<smd name="6" x="2.1496" y="-0.325" dx="1.4709" dy="0.4815" layer="1"/>
<smd name="7" x="2.1496" y="0.325" dx="1.4709" dy="0.4815" layer="1"/>
<smd name="8" x="2.1496" y="0.975" dx="1.4709" dy="0.4815" layer="1"/>
<text x="0" y="2.6047" size="1.27" layer="25" align="bottom-center">&gt;NAME</text>
<text x="0" y="-2.185" size="1.27" layer="27" align="top-center">&gt;VALUE</text>
</package>
<package name="C1206" urn="urn:adsk.eagle:footprint:23125/1">
<description>&lt;b&gt;CAPACITOR&lt;/b&gt;</description>
<wire x1="-2.473" y1="0.983" x2="2.473" y2="0.983" width="0.0508" layer="39"/>
<wire x1="2.473" y1="-0.983" x2="-2.473" y2="-0.983" width="0.0508" layer="39"/>
<wire x1="-2.473" y1="-0.983" x2="-2.473" y2="0.983" width="0.0508" layer="39"/>
<wire x1="2.473" y1="0.983" x2="2.473" y2="-0.983" width="0.0508" layer="39"/>
<wire x1="-0.965" y1="0.787" x2="0.965" y2="0.787" width="0.1016" layer="51"/>
<wire x1="-0.965" y1="-0.787" x2="0.965" y2="-0.787" width="0.1016" layer="51"/>
<smd name="1" x="-1.4" y="0" dx="1.6" dy="1.8" layer="1"/>
<smd name="2" x="1.4" y="0" dx="1.6" dy="1.8" layer="1"/>
<text x="-1.27" y="1.27" size="1.27" layer="25">&gt;NAME</text>
<text x="-1.27" y="-2.54" size="1.27" layer="27">&gt;VALUE</text>
<rectangle x1="-1.7018" y1="-0.8509" x2="-0.9517" y2="0.8491" layer="51"/>
<rectangle x1="0.9517" y1="-0.8491" x2="1.7018" y2="0.8509" layer="51"/>
<rectangle x1="-0.1999" y1="-0.4001" x2="0.1999" y2="0.4001" layer="35"/>
</package>
<package name="R0805" urn="urn:adsk.eagle:footprint:23045/1">
<description>&lt;b&gt;RESISTOR&lt;/b&gt;&lt;p&gt;</description>
<wire x1="-0.41" y1="0.635" x2="0.41" y2="0.635" width="0.1524" layer="51"/>
<wire x1="-0.41" y1="-0.635" x2="0.41" y2="-0.635" width="0.1524" layer="51"/>
<wire x1="-1.973" y1="0.983" x2="1.973" y2="0.983" width="0.0508" layer="39"/>
<wire x1="1.973" y1="0.983" x2="1.973" y2="-0.983" width="0.0508" layer="39"/>
<wire x1="1.973" y1="-0.983" x2="-1.973" y2="-0.983" width="0.0508" layer="39"/>
<wire x1="-1.973" y1="-0.983" x2="-1.973" y2="0.983" width="0.0508" layer="39"/>
<smd name="1" x="-0.95" y="0" dx="1.3" dy="1.5" layer="1"/>
<smd name="2" x="0.95" y="0" dx="1.3" dy="1.5" layer="1"/>
<text x="-0.635" y="1.27" size="1.27" layer="25">&gt;NAME</text>
<text x="-0.635" y="-2.54" size="1.27" layer="27">&gt;VALUE</text>
<rectangle x1="0.4064" y1="-0.6985" x2="1.0564" y2="0.7015" layer="51"/>
<rectangle x1="-1.0668" y1="-0.6985" x2="-0.4168" y2="0.7015" layer="51"/>
<rectangle x1="-0.1999" y1="-0.5001" x2="0.1999" y2="0.5001" layer="35"/>
</package>
<package name="C0805K" urn="urn:adsk.eagle:footprint:23188/1">
<description>&lt;b&gt;Ceramic Chip Capacitor KEMET 0805 reflow solder&lt;/b&gt;&lt;p&gt;
Metric Code Size 2012</description>
<wire x1="-0.925" y1="0.6" x2="0.925" y2="0.6" width="0.1016" layer="51"/>
<wire x1="0.925" y1="-0.6" x2="-0.925" y2="-0.6" width="0.1016" layer="51"/>
<smd name="1" x="-1" y="0" dx="1.3" dy="1.6" layer="1"/>
<smd name="2" x="1" y="0" dx="1.3" dy="1.6" layer="1"/>
<text x="-1" y="0.875" size="1.016" layer="25">&gt;NAME</text>
<text x="-1" y="-1.9" size="1.016" layer="27">&gt;VALUE</text>
<rectangle x1="-1" y1="-0.65" x2="-0.5" y2="0.65" layer="51"/>
<rectangle x1="0.5" y1="-0.65" x2="1" y2="0.65" layer="51"/>
</package>
<package name="R1206" urn="urn:adsk.eagle:footprint:23047/1">
<description>&lt;b&gt;RESISTOR&lt;/b&gt;</description>
<wire x1="0.9525" y1="-0.8128" x2="-0.9652" y2="-0.8128" width="0.1524" layer="51"/>
<wire x1="0.9525" y1="0.8128" x2="-0.9652" y2="0.8128" width="0.1524" layer="51"/>
<wire x1="-2.473" y1="0.983" x2="2.473" y2="0.983" width="0.0508" layer="39"/>
<wire x1="2.473" y1="0.983" x2="2.473" y2="-0.983" width="0.0508" layer="39"/>
<wire x1="2.473" y1="-0.983" x2="-2.473" y2="-0.983" width="0.0508" layer="39"/>
<wire x1="-2.473" y1="-0.983" x2="-2.473" y2="0.983" width="0.0508" layer="39"/>
<smd name="2" x="1.422" y="0" dx="1.6" dy="1.803" layer="1"/>
<smd name="1" x="-1.422" y="0" dx="1.6" dy="1.803" layer="1"/>
<text x="-1.27" y="1.27" size="1.27" layer="25">&gt;NAME</text>
<text x="-1.27" y="-2.54" size="1.27" layer="27">&gt;VALUE</text>
<rectangle x1="-1.6891" y1="-0.8763" x2="-0.9525" y2="0.8763" layer="51"/>
<rectangle x1="0.9525" y1="-0.8763" x2="1.6891" y2="0.8763" layer="51"/>
<rectangle x1="-0.3" y1="-0.7" x2="0.3" y2="0.7" layer="35"/>
</package>
<package name="R0402" urn="urn:adsk.eagle:footprint:25625/1">
<description>&lt;b&gt;RESISTOR&lt;/b&gt;</description>
<wire x1="-0.245" y1="0.224" x2="0.245" y2="0.224" width="0.1524" layer="51"/>
<wire x1="0.245" y1="-0.224" x2="-0.245" y2="-0.224" width="0.1524" layer="51"/>
<wire x1="-1.473" y1="0.483" x2="1.473" y2="0.483" width="0.0508" layer="39"/>
<wire x1="1.473" y1="0.483" x2="1.473" y2="-0.483" width="0.0508" layer="39"/>
<wire x1="1.473" y1="-0.483" x2="-1.473" y2="-0.483" width="0.0508" layer="39"/>
<wire x1="-1.473" y1="-0.483" x2="-1.473" y2="0.483" width="0.0508" layer="39"/>
<smd name="1" x="-0.65" y="0" dx="0.7" dy="0.9" layer="1"/>
<smd name="2" x="0.65" y="0" dx="0.7" dy="0.9" layer="1"/>
<text x="-0.635" y="0.635" size="1.27" layer="25">&gt;NAME</text>
<text x="-0.635" y="-1.905" size="1.27" layer="27">&gt;VALUE</text>
<rectangle x1="-0.554" y1="-0.3048" x2="-0.254" y2="0.2951" layer="51"/>
<rectangle x1="0.2588" y1="-0.3048" x2="0.5588" y2="0.2951" layer="51"/>
<rectangle x1="-0.1999" y1="-0.4001" x2="0.1999" y2="0.4001" layer="35"/>
</package>
<package name="INDPM6060X450N">
<description>&lt;b&gt;VLS6045&lt;/b&gt;&lt;br&gt;
</description>
<smd name="1" x="-2.3" y="0" dx="4.9" dy="2.7" layer="1" rot="R90"/>
<smd name="2" x="2.3" y="0" dx="4.9" dy="2.7" layer="1" rot="R90"/>
<text x="0" y="0" size="1.27" layer="25" align="center">&gt;NAME</text>
<text x="0" y="0" size="1.27" layer="27" align="center">&gt;VALUE</text>
<wire x1="-3.9" y1="3.4" x2="3.9" y2="3.4" width="0.05" layer="51"/>
<wire x1="3.9" y1="3.4" x2="3.9" y2="-3.4" width="0.05" layer="51"/>
<wire x1="3.9" y1="-3.4" x2="-3.9" y2="-3.4" width="0.05" layer="51"/>
<wire x1="-3.9" y1="-3.4" x2="-3.9" y2="3.4" width="0.05" layer="51"/>
<wire x1="-3" y1="3" x2="3" y2="3" width="0.1" layer="51"/>
<wire x1="3" y1="3" x2="3" y2="-3" width="0.1" layer="51"/>
<wire x1="3" y1="-3" x2="-3" y2="-3" width="0.1" layer="51"/>
<wire x1="-3" y1="-3" x2="-3" y2="3" width="0.1" layer="51"/>
<wire x1="3" y1="3" x2="-3" y2="3" width="0.2" layer="21"/>
<wire x1="-3" y1="-3" x2="3" y2="-3" width="0.2" layer="21"/>
</package>
</packages>
<packages3d>
<package3d name="SOP65P490X110-8N" urn="urn:adsk.eagle:package:13422153/1" type="model">
<description>8-SOP, 0.65 mm pitch, 4.90 mm span, 3.00 X 3.00 X 1.10 mm body
&lt;p&gt;8-pin SOP package with 0.65 mm pitch, 4.90 mm span with body size 3.00 X 3.00 X 1.10 mm&lt;/p&gt;</description>
<packageinstances>
<packageinstance name="SOP65P490X110-8N"/>
</packageinstances>
</package3d>
<package3d name="C1206" urn="urn:adsk.eagle:package:23618/2" type="model">
<description>CAPACITOR</description>
<packageinstances>
<packageinstance name="C1206"/>
</packageinstances>
</package3d>
<package3d name="R0805" urn="urn:adsk.eagle:package:23553/2" type="model">
<description>RESISTOR</description>
<packageinstances>
<packageinstance name="R0805"/>
</packageinstances>
</package3d>
<package3d name="C0805K" urn="urn:adsk.eagle:package:23681/2" type="model">
<description>Ceramic Chip Capacitor KEMET 0805 reflow solder
Metric Code Size 2012</description>
<packageinstances>
<packageinstance name="C0805K"/>
</packageinstances>
</package3d>
<package3d name="R1206" urn="urn:adsk.eagle:package:23540/2" type="model">
<description>RESISTOR</description>
<packageinstances>
<packageinstance name="R1206"/>
</packageinstances>
</package3d>
<package3d name="R0402" urn="urn:adsk.eagle:package:26058/2" type="model">
<description>RESISTOR</description>
<packageinstances>
<packageinstance name="R0402"/>
</packageinstances>
</package3d>
</packages3d>
<symbols>
<symbol name="TPS62160DGKR">
<wire x1="5.08" y1="2.54" x2="22.86" y2="2.54" width="0.254" layer="94"/>
<wire x1="22.86" y1="-10.16" x2="22.86" y2="2.54" width="0.254" layer="94"/>
<wire x1="22.86" y1="-10.16" x2="5.08" y2="-10.16" width="0.254" layer="94"/>
<wire x1="5.08" y1="2.54" x2="5.08" y2="-10.16" width="0.254" layer="94"/>
<text x="24.13" y="7.62" size="1.778" layer="95" align="center-left">&gt;NAME</text>
<text x="24.13" y="5.08" size="1.778" layer="96" align="center-left">&gt;VALUE</text>
<pin name="PGND" x="0" y="0" length="middle"/>
<pin name="VIN" x="0" y="-2.54" length="middle"/>
<pin name="EN" x="0" y="-5.08" length="middle"/>
<pin name="AGND" x="0" y="-7.62" length="middle"/>
<pin name="PG" x="27.94" y="0" length="middle" rot="R180"/>
<pin name="SW" x="27.94" y="-2.54" length="middle" rot="R180"/>
<pin name="VOS" x="27.94" y="-5.08" length="middle" rot="R180"/>
<pin name="FB" x="27.94" y="-7.62" length="middle" rot="R180"/>
</symbol>
<symbol name="C-EU">
<wire x1="0" y1="0" x2="0" y2="-0.508" width="0.1524" layer="94"/>
<wire x1="0" y1="-2.54" x2="0" y2="-2.032" width="0.1524" layer="94"/>
<text x="1.524" y="0.381" size="1.778" layer="95">&gt;NAME</text>
<text x="1.524" y="-4.699" size="1.778" layer="96">&gt;VALUE</text>
<rectangle x1="-2.032" y1="-2.032" x2="2.032" y2="-1.524" layer="94"/>
<rectangle x1="-2.032" y1="-1.016" x2="2.032" y2="-0.508" layer="94"/>
<pin name="1" x="0" y="2.54" visible="off" length="short" direction="pas" swaplevel="1" rot="R270"/>
<pin name="2" x="0" y="-5.08" visible="off" length="short" direction="pas" swaplevel="1" rot="R90"/>
</symbol>
<symbol name="R-EU-1-1">
<wire x1="-2.54" y1="-0.889" x2="2.54" y2="-0.889" width="0.254" layer="94"/>
<wire x1="2.54" y1="0.889" x2="-2.54" y2="0.889" width="0.254" layer="94"/>
<wire x1="2.54" y1="-0.889" x2="2.54" y2="0.889" width="0.254" layer="94"/>
<wire x1="-2.54" y1="-0.889" x2="-2.54" y2="0.889" width="0.254" layer="94"/>
<text x="-3.81" y="1.4986" size="1.778" layer="95">&gt;NAME</text>
<text x="-3.81" y="-3.302" size="1.778" layer="96">&gt;VALUE</text>
<pin name="2" x="5.08" y="0" visible="off" length="short" direction="pas" swaplevel="1" rot="R180"/>
<pin name="1" x="-5.08" y="0" visible="off" length="short" direction="pas" swaplevel="1"/>
</symbol>
<symbol name="C-EU-1">
<wire x1="0" y1="0" x2="0" y2="-0.508" width="0.1524" layer="94"/>
<wire x1="0" y1="-2.54" x2="0" y2="-2.032" width="0.1524" layer="94"/>
<text x="1.524" y="0.381" size="1.778" layer="95">&gt;NAME</text>
<text x="1.524" y="-4.699" size="1.778" layer="96">&gt;VALUE</text>
<rectangle x1="-2.032" y1="-2.032" x2="2.032" y2="-1.524" layer="94"/>
<rectangle x1="-2.032" y1="-1.016" x2="2.032" y2="-0.508" layer="94"/>
<pin name="1" x="0" y="2.54" visible="off" length="short" direction="pas" swaplevel="1" rot="R270"/>
<pin name="2" x="0" y="-5.08" visible="off" length="short" direction="pas" swaplevel="1" rot="R90"/>
</symbol>
<symbol name="R-EU">
<wire x1="-2.54" y1="-0.889" x2="2.54" y2="-0.889" width="0.254" layer="94"/>
<wire x1="2.54" y1="0.889" x2="-2.54" y2="0.889" width="0.254" layer="94"/>
<wire x1="2.54" y1="-0.889" x2="2.54" y2="0.889" width="0.254" layer="94"/>
<wire x1="-2.54" y1="-0.889" x2="-2.54" y2="0.889" width="0.254" layer="94"/>
<text x="-3.81" y="1.4986" size="1.778" layer="95">&gt;NAME</text>
<text x="-3.81" y="-3.302" size="1.778" layer="96">&gt;VALUE</text>
<pin name="2" x="5.08" y="0" visible="off" length="short" direction="pas" swaplevel="1" rot="R180"/>
<pin name="1" x="-5.08" y="0" visible="off" length="short" direction="pas" swaplevel="1"/>
</symbol>
<symbol name="R-EU-1">
<wire x1="-2.54" y1="-0.889" x2="2.54" y2="-0.889" width="0.254" layer="94"/>
<wire x1="2.54" y1="0.889" x2="-2.54" y2="0.889" width="0.254" layer="94"/>
<wire x1="2.54" y1="-0.889" x2="2.54" y2="0.889" width="0.254" layer="94"/>
<wire x1="-2.54" y1="-0.889" x2="-2.54" y2="0.889" width="0.254" layer="94"/>
<text x="-3.81" y="1.4986" size="1.778" layer="95">&gt;NAME</text>
<text x="-3.81" y="-3.302" size="1.778" layer="96">&gt;VALUE</text>
<pin name="2" x="5.08" y="0" visible="off" length="short" direction="pas" swaplevel="1" rot="R180"/>
<pin name="1" x="-5.08" y="0" visible="off" length="short" direction="pas" swaplevel="1"/>
</symbol>
<symbol name="VLS6045EX-221M">
<wire x1="5.08" y1="0" x2="7.62" y2="0" width="0.254" layer="94" curve="-175.4"/>
<wire x1="7.62" y1="0" x2="10.16" y2="0" width="0.254" layer="94" curve="-175.4"/>
<wire x1="10.16" y1="0" x2="12.7" y2="0" width="0.254" layer="94" curve="-175.4"/>
<wire x1="12.7" y1="0" x2="15.24" y2="0" width="0.254" layer="94" curve="-175.4"/>
<text x="16.51" y="6.35" size="1.778" layer="95" align="center-left">&gt;NAME</text>
<text x="16.51" y="3.81" size="1.778" layer="96" align="center-left">&gt;VALUE</text>
<pin name="1" x="0" y="0" visible="pad" length="middle"/>
<pin name="2" x="20.32" y="0" visible="pad" length="middle" rot="R180"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="TPS62160DGKR" prefix="IC">
<description>&lt;b&gt;Switching Voltage Regulators 3V-17V 1A Step-Down Converter&lt;/b&gt;&lt;p&gt;
Source: &lt;a href="http://www.ti.com/lit/ds/symlink/tps62160.pdf"&gt; Datasheet &lt;/a&gt;</description>
<gates>
<gate name="G$1" symbol="TPS62160DGKR" x="0" y="0"/>
</gates>
<devices>
<device name="" package="SOP65P490X110-8N">
<connects>
<connect gate="G$1" pin="AGND" pad="4"/>
<connect gate="G$1" pin="EN" pad="3"/>
<connect gate="G$1" pin="FB" pad="5"/>
<connect gate="G$1" pin="PG" pad="8"/>
<connect gate="G$1" pin="PGND" pad="1"/>
<connect gate="G$1" pin="SW" pad="7"/>
<connect gate="G$1" pin="VIN" pad="2"/>
<connect gate="G$1" pin="VOS" pad="6"/>
</connects>
<package3dinstances>
<package3dinstance package3d_urn="urn:adsk.eagle:package:13422153/1"/>
</package3dinstances>
<technologies>
<technology name="">
<attribute name="DESCRIPTION" value="Switching Voltage Regulators 3V-17V 1A Step-Down Converter" constant="no"/>
<attribute name="HEIGHT" value="1.1mm" constant="no"/>
<attribute name="MANUFACTURER_NAME" value="Texas Instruments" constant="no"/>
<attribute name="MANUFACTURER_PART_NUMBER" value="TPS62160DGKR" constant="no"/>
<attribute name="MOUSER_PART_NUMBER" value="595-TPS62160DGKR" constant="no"/>
<attribute name="MOUSER_PRICE-STOCK" value="https://www.mouser.com/Search/Refine.aspx?Keyword=595-TPS62160DGKR" constant="no"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="CNC5L1X7R1C106K160AE" prefix="C" uservalue="yes">
<description>&lt;B&gt;CAPACITOR&lt;/B&gt;, European symbol</description>
<gates>
<gate name="G$1" symbol="C-EU" x="0" y="0"/>
</gates>
<devices>
<device name="C1206" package="C1206">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
</connects>
<package3dinstances>
<package3dinstance package3d_urn="urn:adsk.eagle:package:23618/2"/>
</package3dinstances>
<technologies>
<technology name="">
<attribute name="MAX" value="16V" constant="no"/>
<attribute name="POPULARITY" value="54" constant="no"/>
<attribute name="SPICEPREFIX" value="C" constant="no"/>
<attribute name="VALUE" value="10uF" constant="no"/>
</technology>
</technologies>
</device>
</devices>
<spice>
<pinmapping spiceprefix="C">
<pinmap gate="G$1" pin="1" pinorder="1"/>
<pinmap gate="G$1" pin="2" pinorder="2"/>
</pinmapping>
</spice>
</deviceset>
<deviceset name="ERJ-PB6B5623V" prefix="R" uservalue="yes">
<description>&lt;B&gt;RESISTOR&lt;/B&gt;, European symbol</description>
<gates>
<gate name="G$1" symbol="R-EU-1-1" x="0" y="0"/>
</gates>
<devices>
<device name="R0805" package="R0805">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
</connects>
<package3dinstances>
<package3dinstance package3d_urn="urn:adsk.eagle:package:23553/2"/>
</package3dinstances>
<technologies>
<technology name="">
<attribute name="VALUE" value="562kOhm" constant="no"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="GRM219R61C226ME15K" prefix="C" uservalue="yes">
<description>&lt;B&gt;CAPACITOR&lt;/B&gt;, European symbol</description>
<gates>
<gate name="G$1" symbol="C-EU-1" x="0" y="0"/>
</gates>
<devices>
<device name="C0805K" package="C0805K">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
</connects>
<package3dinstances>
<package3dinstance package3d_urn="urn:adsk.eagle:package:23681/2"/>
</package3dinstances>
<technologies>
<technology name="">
<attribute name="MAXV" value="16V" constant="no"/>
<attribute name="POPULARITY" value="52" constant="no"/>
<attribute name="SPICEPREFIX" value="C" constant="no"/>
<attribute name="VALUE" value="22uF" constant="no"/>
</technology>
</technologies>
</device>
</devices>
<spice>
<pinmapping spiceprefix="C">
<pinmap gate="G$1" pin="1" pinorder="1"/>
<pinmap gate="G$1" pin="2" pinorder="2"/>
</pinmapping>
</spice>
</deviceset>
<deviceset name="RCS1206100KJNEA" prefix="R" uservalue="yes">
<description>&lt;B&gt;RESISTOR&lt;/B&gt;, European symbol</description>
<gates>
<gate name="G$1" symbol="R-EU" x="0" y="0"/>
</gates>
<devices>
<device name="R1206" package="R1206">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
</connects>
<package3dinstances>
<package3dinstance package3d_urn="urn:adsk.eagle:package:23540/2"/>
</package3dinstances>
<technologies>
<technology name="">
<attribute name="POPULARITY" value="41" constant="no"/>
<attribute name="SPICEPREFIX" value="R" constant="no"/>
<attribute name="VALUE" value="100kOhm" constant="no"/>
<attribute name="WATT" value="0.5" constant="no"/>
</technology>
</technologies>
</device>
</devices>
<spice>
<pinmapping spiceprefix="R">
<pinmap gate="G$1" pin="1" pinorder="1"/>
<pinmap gate="G$1" pin="2" pinorder="2"/>
</pinmapping>
</spice>
</deviceset>
<deviceset name="RN73R1ETTP1803D25" prefix="R" uservalue="yes">
<description>&lt;B&gt;RESISTOR&lt;/B&gt;, European symbol</description>
<gates>
<gate name="G$1" symbol="R-EU-1" x="0" y="0"/>
</gates>
<devices>
<device name="R0402" package="R0402">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
</connects>
<package3dinstances>
<package3dinstance package3d_urn="urn:adsk.eagle:package:26058/2"/>
</package3dinstances>
<technologies>
<technology name="">
<attribute name="VALUE" value="180kOhm" constant="no"/>
<attribute name="WATT" value="0.5" constant="no"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="VLS6045EX-221M" prefix="L">
<description>&lt;b&gt;Wirewound Inductor 220uH 500mA 1.15 mOhm TDK VLS-EX Series Shielded Wire-wound SMD Inductor with a Ferrite Core, 220 uH Wire-Wound 500mA Idc&lt;/b&gt;&lt;p&gt;
Source: &lt;a href="https://product.tdk.com/info/en/catalog/datasheets/inductor_commercial_power_vls6045ex_en.pdf"&gt; Datasheet &lt;/a&gt;</description>
<gates>
<gate name="G$1" symbol="VLS6045EX-221M" x="0" y="0"/>
</gates>
<devices>
<device name="" package="INDPM6060X450N">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
</connects>
<technologies>
<technology name="">
<attribute name="DESCRIPTION" value="Wirewound Inductor 220uH 500mA 1.15 mOhm TDK VLS-EX Series Shielded Wire-wound SMD Inductor with a Ferrite Core, 220 uH Wire-Wound 500mA Idc" constant="no"/>
<attribute name="HEIGHT" value="4.5mm" constant="no"/>
<attribute name="MANUFACTURER_NAME" value="TDK" constant="no"/>
<attribute name="MANUFACTURER_PART_NUMBER" value="VLS6045EX-221M" constant="no"/>
<attribute name="MOUSER_PART_NUMBER" value="810-VLS6045EX-221M" constant="no"/>
<attribute name="MOUSER_PRICE-STOCK" value="https://www.mouser.com/Search/Refine.aspx?Keyword=810-VLS6045EX-221M" constant="no"/>
<attribute name="RS_PART_NUMBER" value="1247062" constant="no"/>
<attribute name="RS_PRICE-STOCK" value="http://uk.rs-online.com/web/p/products/1247062" constant="no"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="supply1" urn="urn:adsk.eagle:library:371">
<description>&lt;b&gt;Supply Symbols&lt;/b&gt;&lt;p&gt;
 GND, VCC, 0V, +5V, -5V, etc.&lt;p&gt;
 Please keep in mind, that these devices are necessary for the
 automatic wiring of the supply signals.&lt;p&gt;
 The pin name defined in the symbol is identical to the net which is to be wired automatically.&lt;p&gt;
 In this library the device names are the same as the pin names of the symbols, therefore the correct signal names appear next to the supply symbols in the schematic.&lt;p&gt;
 &lt;author&gt;Created by librarian@cadsoft.de&lt;/author&gt;</description>
<packages>
</packages>
<symbols>
<symbol name="AGND" urn="urn:adsk.eagle:symbol:26949/1" library_version="1">
<wire x1="-1.905" y1="0" x2="1.905" y2="0" width="0.254" layer="94"/>
<wire x1="-1.0922" y1="-0.508" x2="1.0922" y2="-0.508" width="0.254" layer="94"/>
<text x="-2.54" y="-5.08" size="1.778" layer="96" rot="R90">&gt;VALUE</text>
<pin name="AGND" x="0" y="2.54" visible="off" length="short" direction="sup" rot="R270"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="AGND" urn="urn:adsk.eagle:component:26977/1" prefix="AGND" library_version="1">
<description>&lt;b&gt;SUPPLY SYMBOL&lt;/b&gt;</description>
<gates>
<gate name="VR1" symbol="AGND" x="0" y="0"/>
</gates>
<devices>
<device name="">
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="pinhead" urn="urn:adsk.eagle:library:325">
<description>&lt;b&gt;Pin Header Connectors&lt;/b&gt;&lt;p&gt;
&lt;author&gt;Created by librarian@cadsoft.de&lt;/author&gt;</description>
<packages>
<package name="1X04" urn="urn:adsk.eagle:footprint:22258/1" library_version="3">
<description>&lt;b&gt;PIN HEADER&lt;/b&gt;</description>
<wire x1="0" y1="0.635" x2="0.635" y2="1.27" width="0.1524" layer="21"/>
<wire x1="0.635" y1="1.27" x2="1.905" y2="1.27" width="0.1524" layer="21"/>
<wire x1="1.905" y1="1.27" x2="2.54" y2="0.635" width="0.1524" layer="21"/>
<wire x1="2.54" y1="0.635" x2="2.54" y2="-0.635" width="0.1524" layer="21"/>
<wire x1="2.54" y1="-0.635" x2="1.905" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="1.905" y1="-1.27" x2="0.635" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="0.635" y1="-1.27" x2="0" y2="-0.635" width="0.1524" layer="21"/>
<wire x1="-4.445" y1="1.27" x2="-3.175" y2="1.27" width="0.1524" layer="21"/>
<wire x1="-3.175" y1="1.27" x2="-2.54" y2="0.635" width="0.1524" layer="21"/>
<wire x1="-2.54" y1="0.635" x2="-2.54" y2="-0.635" width="0.1524" layer="21"/>
<wire x1="-2.54" y1="-0.635" x2="-3.175" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="-2.54" y1="0.635" x2="-1.905" y2="1.27" width="0.1524" layer="21"/>
<wire x1="-1.905" y1="1.27" x2="-0.635" y2="1.27" width="0.1524" layer="21"/>
<wire x1="-0.635" y1="1.27" x2="0" y2="0.635" width="0.1524" layer="21"/>
<wire x1="0" y1="0.635" x2="0" y2="-0.635" width="0.1524" layer="21"/>
<wire x1="0" y1="-0.635" x2="-0.635" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="-0.635" y1="-1.27" x2="-1.905" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="-1.905" y1="-1.27" x2="-2.54" y2="-0.635" width="0.1524" layer="21"/>
<wire x1="-5.08" y1="0.635" x2="-5.08" y2="-0.635" width="0.1524" layer="21"/>
<wire x1="-4.445" y1="1.27" x2="-5.08" y2="0.635" width="0.1524" layer="21"/>
<wire x1="-5.08" y1="-0.635" x2="-4.445" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="-3.175" y1="-1.27" x2="-4.445" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="3.175" y1="1.27" x2="4.445" y2="1.27" width="0.1524" layer="21"/>
<wire x1="4.445" y1="1.27" x2="5.08" y2="0.635" width="0.1524" layer="21"/>
<wire x1="5.08" y1="0.635" x2="5.08" y2="-0.635" width="0.1524" layer="21"/>
<wire x1="5.08" y1="-0.635" x2="4.445" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="3.175" y1="1.27" x2="2.54" y2="0.635" width="0.1524" layer="21"/>
<wire x1="2.54" y1="-0.635" x2="3.175" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="4.445" y1="-1.27" x2="3.175" y2="-1.27" width="0.1524" layer="21"/>
<pad name="1" x="-3.81" y="0" drill="1.016" shape="long" rot="R90"/>
<pad name="2" x="-1.27" y="0" drill="1.016" shape="long" rot="R90"/>
<pad name="3" x="1.27" y="0" drill="1.016" shape="long" rot="R90"/>
<pad name="4" x="3.81" y="0" drill="1.016" shape="long" rot="R90"/>
<text x="-5.1562" y="1.8288" size="1.27" layer="25" ratio="10">&gt;NAME</text>
<text x="-5.08" y="-3.175" size="1.27" layer="27">&gt;VALUE</text>
<rectangle x1="1.016" y1="-0.254" x2="1.524" y2="0.254" layer="51"/>
<rectangle x1="-1.524" y1="-0.254" x2="-1.016" y2="0.254" layer="51"/>
<rectangle x1="-4.064" y1="-0.254" x2="-3.556" y2="0.254" layer="51"/>
<rectangle x1="3.556" y1="-0.254" x2="4.064" y2="0.254" layer="51"/>
</package>
<package name="1X04/90" urn="urn:adsk.eagle:footprint:22259/1" library_version="3">
<description>&lt;b&gt;PIN HEADER&lt;/b&gt;</description>
<wire x1="-5.08" y1="-1.905" x2="-2.54" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="-2.54" y1="-1.905" x2="-2.54" y2="0.635" width="0.1524" layer="21"/>
<wire x1="-2.54" y1="0.635" x2="-5.08" y2="0.635" width="0.1524" layer="21"/>
<wire x1="-5.08" y1="0.635" x2="-5.08" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="-3.81" y1="6.985" x2="-3.81" y2="1.27" width="0.762" layer="21"/>
<wire x1="-2.54" y1="-1.905" x2="0" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="0" y1="-1.905" x2="0" y2="0.635" width="0.1524" layer="21"/>
<wire x1="0" y1="0.635" x2="-2.54" y2="0.635" width="0.1524" layer="21"/>
<wire x1="-1.27" y1="6.985" x2="-1.27" y2="1.27" width="0.762" layer="21"/>
<wire x1="0" y1="-1.905" x2="2.54" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="2.54" y1="-1.905" x2="2.54" y2="0.635" width="0.1524" layer="21"/>
<wire x1="2.54" y1="0.635" x2="0" y2="0.635" width="0.1524" layer="21"/>
<wire x1="1.27" y1="6.985" x2="1.27" y2="1.27" width="0.762" layer="21"/>
<wire x1="2.54" y1="-1.905" x2="5.08" y2="-1.905" width="0.1524" layer="21"/>
<wire x1="5.08" y1="-1.905" x2="5.08" y2="0.635" width="0.1524" layer="21"/>
<wire x1="5.08" y1="0.635" x2="2.54" y2="0.635" width="0.1524" layer="21"/>
<wire x1="3.81" y1="6.985" x2="3.81" y2="1.27" width="0.762" layer="21"/>
<pad name="1" x="-3.81" y="-3.81" drill="1.016" shape="long" rot="R90"/>
<pad name="2" x="-1.27" y="-3.81" drill="1.016" shape="long" rot="R90"/>
<pad name="3" x="1.27" y="-3.81" drill="1.016" shape="long" rot="R90"/>
<pad name="4" x="3.81" y="-3.81" drill="1.016" shape="long" rot="R90"/>
<text x="-5.715" y="-3.81" size="1.27" layer="25" ratio="10" rot="R90">&gt;NAME</text>
<text x="6.985" y="-4.445" size="1.27" layer="27" rot="R90">&gt;VALUE</text>
<rectangle x1="-4.191" y1="0.635" x2="-3.429" y2="1.143" layer="21"/>
<rectangle x1="-1.651" y1="0.635" x2="-0.889" y2="1.143" layer="21"/>
<rectangle x1="0.889" y1="0.635" x2="1.651" y2="1.143" layer="21"/>
<rectangle x1="3.429" y1="0.635" x2="4.191" y2="1.143" layer="21"/>
<rectangle x1="-4.191" y1="-2.921" x2="-3.429" y2="-1.905" layer="21"/>
<rectangle x1="-1.651" y1="-2.921" x2="-0.889" y2="-1.905" layer="21"/>
<rectangle x1="0.889" y1="-2.921" x2="1.651" y2="-1.905" layer="21"/>
<rectangle x1="3.429" y1="-2.921" x2="4.191" y2="-1.905" layer="21"/>
</package>
</packages>
<packages3d>
<package3d name="1X04" urn="urn:adsk.eagle:package:22407/2" type="model" library_version="3">
<description>PIN HEADER</description>
<packageinstances>
<packageinstance name="1X04"/>
</packageinstances>
</package3d>
<package3d name="1X04/90" urn="urn:adsk.eagle:package:22404/2" type="model" library_version="3">
<description>PIN HEADER</description>
<packageinstances>
<packageinstance name="1X04/90"/>
</packageinstances>
</package3d>
</packages3d>
<symbols>
<symbol name="PINHD4" urn="urn:adsk.eagle:symbol:22257/1" library_version="3">
<wire x1="-6.35" y1="-5.08" x2="1.27" y2="-5.08" width="0.4064" layer="94"/>
<wire x1="1.27" y1="-5.08" x2="1.27" y2="7.62" width="0.4064" layer="94"/>
<wire x1="1.27" y1="7.62" x2="-6.35" y2="7.62" width="0.4064" layer="94"/>
<wire x1="-6.35" y1="7.62" x2="-6.35" y2="-5.08" width="0.4064" layer="94"/>
<text x="-6.35" y="8.255" size="1.778" layer="95">&gt;NAME</text>
<text x="-6.35" y="-7.62" size="1.778" layer="96">&gt;VALUE</text>
<pin name="1" x="-2.54" y="5.08" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="2" x="-2.54" y="2.54" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="3" x="-2.54" y="0" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="4" x="-2.54" y="-2.54" visible="pad" length="short" direction="pas" function="dot"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="PINHD-1X4" urn="urn:adsk.eagle:component:22499/4" prefix="JP" uservalue="yes" library_version="3">
<description>&lt;b&gt;PIN HEADER&lt;/b&gt;</description>
<gates>
<gate name="A" symbol="PINHD4" x="0" y="0"/>
</gates>
<devices>
<device name="" package="1X04">
<connects>
<connect gate="A" pin="1" pad="1"/>
<connect gate="A" pin="2" pad="2"/>
<connect gate="A" pin="3" pad="3"/>
<connect gate="A" pin="4" pad="4"/>
</connects>
<package3dinstances>
<package3dinstance package3d_urn="urn:adsk.eagle:package:22407/2"/>
</package3dinstances>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="/90" package="1X04/90">
<connects>
<connect gate="A" pin="1" pad="1"/>
<connect gate="A" pin="2" pad="2"/>
<connect gate="A" pin="3" pad="3"/>
<connect gate="A" pin="4" pad="4"/>
</connects>
<package3dinstances>
<package3dinstance package3d_urn="urn:adsk.eagle:package:22404/2"/>
</package3dinstances>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="CommonParts">
<packages>
<package name="JST_S2B-PH-SM4-TB(LF)(SN)">
<wire x1="3.95" y1="0.35" x2="1.9" y2="0.35" width="0.127" layer="21"/>
<wire x1="-1.9" y1="0.35" x2="-3.95" y2="0.35" width="0.127" layer="21"/>
<wire x1="-3.95" y1="0.35" x2="-3.95" y2="-3.65" width="0.127" layer="21"/>
<circle x="-2.15" y="1.35" radius="0.1" width="0.3" layer="21"/>
<wire x1="-2.2" y1="-7.25" x2="2.2" y2="-7.25" width="0.127" layer="21"/>
<wire x1="3.95" y1="-3.65" x2="3.95" y2="0.35" width="0.127" layer="21"/>
<wire x1="4.35" y1="-7.7" x2="-4.35" y2="-7.7" width="0.05" layer="39"/>
<wire x1="-4.35" y1="-7.7" x2="-4.35" y2="0.6" width="0.05" layer="39"/>
<wire x1="-4.35" y1="0.6" x2="-1.75" y2="0.6" width="0.05" layer="39"/>
<wire x1="-1.75" y1="0.6" x2="-1.75" y2="2" width="0.05" layer="39"/>
<wire x1="-1.75" y1="2" x2="1.75" y2="2" width="0.05" layer="39"/>
<wire x1="1.75" y1="2" x2="1.75" y2="0.6" width="0.05" layer="39"/>
<wire x1="1.75" y1="0.6" x2="4.35" y2="0.6" width="0.05" layer="39"/>
<wire x1="4.35" y1="0.6" x2="4.35" y2="-7.7" width="0.05" layer="39"/>
<text x="-3.87423125" y="2.547609375" size="1.27278125" layer="25">&gt;NAME</text>
<text x="-3.75341875" y="-10.1187" size="1.274540625" layer="27">&gt;VALUE</text>
<wire x1="3.95" y1="0.35" x2="3.95" y2="-7.25" width="0.127" layer="51"/>
<wire x1="3.95" y1="-7.25" x2="-3.95" y2="-7.25" width="0.127" layer="51"/>
<wire x1="-3.95" y1="-7.25" x2="-3.95" y2="0.35" width="0.127" layer="51"/>
<wire x1="-3.95" y1="0.35" x2="3.95" y2="0.35" width="0.127" layer="51"/>
<circle x="-2.15" y="1.35" radius="0.1" width="0.3" layer="51"/>
<smd name="2" x="1" y="0" dx="1" dy="3.5" layer="1" rot="R180"/>
<smd name="1" x="-1" y="0" dx="1" dy="3.5" layer="1" rot="R180"/>
<smd name="S2" x="3.35" y="-5.75" dx="1.5" dy="3.4" layer="1" rot="R180"/>
<smd name="S1" x="-3.35" y="-5.75" dx="1.5" dy="3.4" layer="1" rot="R180"/>
</package>
</packages>
<symbols>
<symbol name="S2B-PH-SM4-TB(LF)(SN)">
<text x="-5.0829" y="5.59571875" size="1.272509375" layer="95">&gt;NAME</text>
<text x="-5.08811875" y="-9.42268125" size="1.275409375" layer="96">&gt;VALUE</text>
<wire x1="-5.08" y1="5.08" x2="5.08" y2="5.08" width="0.1524" layer="94"/>
<wire x1="5.08" y1="5.08" x2="5.08" y2="-7.62" width="0.1524" layer="94"/>
<wire x1="5.08" y1="-7.62" x2="-5.08" y2="-7.62" width="0.1524" layer="94"/>
<wire x1="-5.08" y1="-7.62" x2="-5.08" y2="5.08" width="0.1524" layer="94"/>
<pin name="1" x="-7.62" y="2.54" length="short" direction="pas"/>
<pin name="2" x="-7.62" y="0" length="short" direction="pas"/>
<pin name="SHIELD" x="-7.62" y="-5.08" length="short" direction="pas"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="S2B-PH-SM4" prefix="J">
<description>PH Series 2 Position 2 mm Pitch Surface Mount Side Entry Shrouded Header</description>
<gates>
<gate name="G$1" symbol="S2B-PH-SM4-TB(LF)(SN)" x="0" y="0"/>
</gates>
<devices>
<device name="" package="JST_S2B-PH-SM4-TB(LF)(SN)">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
<connect gate="G$1" pin="SHIELD" pad="S1 S2"/>
</connects>
<technologies>
<technology name="">
<attribute name="DESCRIPTION" value=" PH Series 2 Position 2 mm Pitch Surface Mount Side Entry Shrouded Header "/>
<attribute name="DIGI-KEY_PART_NUMBER" value="455-1749-1-ND"/>
<attribute name="DIGI-KEY_PURCHASE_URL" value="https://www.digikey.com/product-detail/en/jst-sales-america-inc/S2B-PH-SM4-TB_LF__SN_/455-1749-1-ND/926846?utm_source=snapeda&amp;utm_medium=aggregator&amp;utm_campaign=symbol"/>
<attribute name="MF" value="JST Sales"/>
<attribute name="MP" value="S2B-PH-SM4-TB_LF__SN_"/>
<attribute name="PACKAGE" value="None"/>
</technology>
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
<part name="IC1" library="MouserUseful" deviceset="TPS62160DGKR" device="" package3d_urn="urn:adsk.eagle:package:13422153/1"/>
<part name="C1" library="MouserUseful" deviceset="CNC5L1X7R1C106K160AE" device="C1206" package3d_urn="urn:adsk.eagle:package:23618/2" value="10uF"/>
<part name="R2" library="MouserUseful" deviceset="ERJ-PB6B5623V" device="R0805" package3d_urn="urn:adsk.eagle:package:23553/2" value="562kOhm"/>
<part name="C2" library="MouserUseful" deviceset="GRM219R61C226ME15K" device="C0805K" package3d_urn="urn:adsk.eagle:package:23681/2" value="22uF"/>
<part name="R1" library="MouserUseful" deviceset="RCS1206100KJNEA" device="R1206" package3d_urn="urn:adsk.eagle:package:23540/2" value="100kOhm"/>
<part name="R3" library="MouserUseful" deviceset="RN73R1ETTP1803D25" device="R0402" package3d_urn="urn:adsk.eagle:package:26058/2" value="180kOhm"/>
<part name="AGND1" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="AGND" device=""/>
<part name="JP1" library="pinhead" library_urn="urn:adsk.eagle:library:325" deviceset="PINHD-1X4" device="" package3d_urn="urn:adsk.eagle:package:22407/2"/>
<part name="AGND4" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="AGND" device=""/>
<part name="AGND5" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="AGND" device=""/>
<part name="J1" library="CommonParts" deviceset="S2B-PH-SM4" device=""/>
<part name="J2" library="CommonParts" deviceset="S2B-PH-SM4" device=""/>
<part name="AGND2" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="AGND" device=""/>
<part name="AGND3" library="supply1" library_urn="urn:adsk.eagle:library:371" deviceset="AGND" device=""/>
<part name="L1" library="MouserUseful" deviceset="VLS6045EX-221M" device=""/>
</parts>
<sheets>
<sheet>
<plain>
<frame x1="43.18" y1="81.28" x2="167.64" y2="167.64" columns="8" rows="5" layer="91"/>
</plain>
<instances>
<instance part="IC1" gate="G$1" x="76.2" y="144.78" smashed="yes">
<attribute name="NAME" x="80.01" y="152.4" size="1.778" layer="95" align="center-left"/>
<attribute name="VALUE" x="80.01" y="149.86" size="1.778" layer="96" align="center-left"/>
</instance>
<instance part="C1" gate="G$1" x="55.88" y="144.78" smashed="yes">
<attribute name="NAME" x="57.404" y="145.161" size="1.778" layer="95"/>
<attribute name="VALUE" x="57.404" y="140.081" size="1.778" layer="96"/>
</instance>
<instance part="R2" gate="G$1" x="134.62" y="144.78" smashed="yes" rot="R90">
<attribute name="NAME" x="133.1214" y="140.97" size="1.778" layer="95" rot="R90"/>
<attribute name="VALUE" x="137.922" y="140.97" size="1.778" layer="96" rot="R90"/>
</instance>
<instance part="C2" gate="G$1" x="147.32" y="144.78" smashed="yes">
<attribute name="NAME" x="148.844" y="145.161" size="1.778" layer="95"/>
<attribute name="VALUE" x="148.844" y="140.081" size="1.778" layer="96"/>
</instance>
<instance part="R1" gate="G$1" x="116.84" y="144.78" smashed="yes">
<attribute name="NAME" x="113.03" y="146.2786" size="1.778" layer="95"/>
<attribute name="VALUE" x="113.03" y="141.478" size="1.778" layer="96"/>
</instance>
<instance part="R3" gate="G$1" x="134.62" y="129.54" smashed="yes" rot="R90">
<attribute name="NAME" x="133.1214" y="125.73" size="1.778" layer="95" rot="R90"/>
<attribute name="VALUE" x="137.922" y="125.73" size="1.778" layer="96" rot="R90"/>
</instance>
<instance part="AGND1" gate="VR1" x="81.28" y="114.3" smashed="yes">
<attribute name="VALUE" x="78.74" y="109.22" size="1.778" layer="96" rot="R90"/>
</instance>
<instance part="JP1" gate="A" x="142.24" y="114.3" smashed="yes" rot="R90">
<attribute name="NAME" x="133.985" y="107.95" size="1.778" layer="95" rot="R90"/>
<attribute name="VALUE" x="149.86" y="107.95" size="1.778" layer="96" rot="R90"/>
</instance>
<instance part="AGND4" gate="VR1" x="139.7" y="93.98" smashed="yes">
<attribute name="VALUE" x="137.16" y="88.9" size="1.778" layer="96" rot="R90"/>
</instance>
<instance part="AGND5" gate="VR1" x="142.24" y="93.98" smashed="yes">
<attribute name="VALUE" x="139.7" y="88.9" size="1.778" layer="96" rot="R90"/>
</instance>
<instance part="J1" gate="G$1" x="104.14" y="111.76" smashed="yes" rot="R90">
<attribute name="NAME" x="98.54428125" y="106.6771" size="1.272509375" layer="95" rot="R90"/>
<attribute name="VALUE" x="113.56268125" y="106.67188125" size="1.275409375" layer="96" rot="R90"/>
</instance>
<instance part="J2" gate="G$1" x="121.92" y="111.76" smashed="yes" rot="R90">
<attribute name="NAME" x="116.32428125" y="106.6771" size="1.272509375" layer="95" rot="R90"/>
<attribute name="VALUE" x="131.34268125" y="106.67188125" size="1.275409375" layer="96" rot="R90"/>
</instance>
<instance part="AGND2" gate="VR1" x="109.22" y="91.44" smashed="yes">
<attribute name="VALUE" x="106.68" y="86.36" size="1.778" layer="96" rot="R90"/>
</instance>
<instance part="AGND3" gate="VR1" x="127" y="91.44" smashed="yes">
<attribute name="VALUE" x="124.46" y="86.36" size="1.778" layer="96" rot="R90"/>
</instance>
<instance part="L1" gate="G$1" x="106.68" y="154.94" smashed="yes">
<attribute name="NAME" x="123.19" y="161.29" size="1.778" layer="95" align="center-left"/>
<attribute name="VALUE" x="123.19" y="158.75" size="1.778" layer="96" align="center-left"/>
</instance>
</instances>
<busses>
</busses>
<nets>
<net name="VIN" class="0">
<segment>
<pinref part="IC1" gate="G$1" pin="VIN"/>
<wire x1="76.2" y1="142.24" x2="71.12" y2="142.24" width="0.1524" layer="91"/>
<pinref part="IC1" gate="G$1" pin="EN"/>
<wire x1="76.2" y1="139.7" x2="71.12" y2="139.7" width="0.1524" layer="91"/>
<wire x1="71.12" y1="139.7" x2="71.12" y2="142.24" width="0.1524" layer="91"/>
<junction x="71.12" y="142.24"/>
<wire x1="55.88" y1="152.4" x2="71.12" y2="152.4" width="0.1524" layer="91"/>
<wire x1="71.12" y1="152.4" x2="71.12" y2="142.24" width="0.1524" layer="91"/>
<pinref part="C1" gate="G$1" pin="1"/>
<wire x1="55.88" y1="147.32" x2="55.88" y2="152.4" width="0.1524" layer="91"/>
<wire x1="55.88" y1="152.4" x2="53.34" y2="152.4" width="0.1524" layer="91"/>
<junction x="55.88" y="152.4"/>
<label x="53.34" y="152.4" size="1.778" layer="95" rot="R180" xref="yes"/>
</segment>
<segment>
<pinref part="JP1" gate="A" pin="1"/>
<wire x1="137.16" y1="111.76" x2="137.16" y2="106.68" width="0.1524" layer="91"/>
<label x="137.16" y="106.68" size="1.778" layer="95" rot="R270" xref="yes"/>
</segment>
<segment>
<pinref part="J1" gate="G$1" pin="1"/>
<wire x1="101.6" y1="104.14" x2="101.6" y2="101.6" width="0.1524" layer="91"/>
<label x="101.6" y="101.6" size="1.778" layer="95" rot="R270" xref="yes"/>
</segment>
</net>
<net name="IOUT" class="0">
<segment>
<pinref part="IC1" gate="G$1" pin="VOS"/>
<wire x1="104.14" y1="139.7" x2="127" y2="139.7" width="0.1524" layer="91"/>
<wire x1="127" y1="139.7" x2="127" y2="144.78" width="0.1524" layer="91"/>
<pinref part="R1" gate="G$1" pin="2"/>
<wire x1="121.92" y1="144.78" x2="127" y2="144.78" width="0.1524" layer="91"/>
<wire x1="127" y1="154.94" x2="134.62" y2="154.94" width="0.1524" layer="91"/>
<wire x1="134.62" y1="154.94" x2="147.32" y2="154.94" width="0.1524" layer="91"/>
<wire x1="147.32" y1="154.94" x2="152.4" y2="154.94" width="0.1524" layer="91"/>
<wire x1="127" y1="144.78" x2="127" y2="154.94" width="0.1524" layer="91"/>
<junction x="127" y="144.78"/>
<pinref part="R2" gate="G$1" pin="2"/>
<wire x1="134.62" y1="149.86" x2="134.62" y2="154.94" width="0.1524" layer="91"/>
<junction x="134.62" y="154.94"/>
<pinref part="C2" gate="G$1" pin="1"/>
<wire x1="147.32" y1="147.32" x2="147.32" y2="154.94" width="0.1524" layer="91"/>
<junction x="147.32" y="154.94"/>
<label x="152.4" y="154.94" size="1.778" layer="95" xref="yes"/>
<pinref part="L1" gate="G$1" pin="2"/>
<junction x="127" y="154.94"/>
</segment>
<segment>
<pinref part="JP1" gate="A" pin="4"/>
<wire x1="144.78" y1="111.76" x2="144.78" y2="106.68" width="0.1524" layer="91"/>
<label x="144.78" y="106.68" size="1.778" layer="95" rot="R270" xref="yes"/>
</segment>
<segment>
<pinref part="J2" gate="G$1" pin="1"/>
<wire x1="119.38" y1="104.14" x2="119.38" y2="101.6" width="0.1524" layer="91"/>
<label x="119.38" y="101.6" size="1.778" layer="95" rot="R270" xref="yes"/>
</segment>
</net>
<net name="AGND" class="0">
<segment>
<pinref part="IC1" gate="G$1" pin="PGND"/>
<wire x1="76.2" y1="144.78" x2="66.04" y2="144.78" width="0.1524" layer="91"/>
<pinref part="R3" gate="G$1" pin="1"/>
<wire x1="134.62" y1="124.46" x2="134.62" y2="121.92" width="0.1524" layer="91"/>
<pinref part="C2" gate="G$1" pin="2"/>
<wire x1="134.62" y1="121.92" x2="147.32" y2="121.92" width="0.1524" layer="91"/>
<wire x1="147.32" y1="121.92" x2="147.32" y2="139.7" width="0.1524" layer="91"/>
<pinref part="IC1" gate="G$1" pin="AGND"/>
<wire x1="76.2" y1="137.16" x2="68.58" y2="137.16" width="0.1524" layer="91"/>
<wire x1="134.62" y1="121.92" x2="81.28" y2="121.92" width="0.1524" layer="91"/>
<wire x1="81.28" y1="121.92" x2="68.58" y2="121.92" width="0.1524" layer="91"/>
<wire x1="68.58" y1="121.92" x2="68.58" y2="137.16" width="0.1524" layer="91"/>
<junction x="134.62" y="121.92"/>
<wire x1="66.04" y1="144.78" x2="66.04" y2="121.92" width="0.1524" layer="91"/>
<wire x1="66.04" y1="121.92" x2="68.58" y2="121.92" width="0.1524" layer="91"/>
<junction x="68.58" y="121.92"/>
<pinref part="C1" gate="G$1" pin="2"/>
<wire x1="55.88" y1="139.7" x2="55.88" y2="121.92" width="0.1524" layer="91"/>
<wire x1="55.88" y1="121.92" x2="66.04" y2="121.92" width="0.1524" layer="91"/>
<junction x="66.04" y="121.92"/>
<pinref part="AGND1" gate="VR1" pin="AGND"/>
<wire x1="81.28" y1="116.84" x2="81.28" y2="121.92" width="0.1524" layer="91"/>
<junction x="81.28" y="121.92"/>
</segment>
<segment>
<pinref part="JP1" gate="A" pin="2"/>
<pinref part="AGND4" gate="VR1" pin="AGND"/>
<wire x1="139.7" y1="96.52" x2="139.7" y2="111.76" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="JP1" gate="A" pin="3"/>
<pinref part="AGND5" gate="VR1" pin="AGND"/>
<wire x1="142.24" y1="96.52" x2="142.24" y2="111.76" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="J1" gate="G$1" pin="2"/>
<wire x1="104.14" y1="104.14" x2="104.14" y2="93.98" width="0.1524" layer="91"/>
<pinref part="AGND2" gate="VR1" pin="AGND"/>
<wire x1="104.14" y1="93.98" x2="109.22" y2="93.98" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="J2" gate="G$1" pin="2"/>
<pinref part="AGND3" gate="VR1" pin="AGND"/>
<wire x1="121.92" y1="104.14" x2="121.92" y2="93.98" width="0.1524" layer="91"/>
<wire x1="121.92" y1="93.98" x2="127" y2="93.98" width="0.1524" layer="91"/>
</segment>
</net>
<net name="PG2" class="0">
<segment>
<pinref part="IC1" gate="G$1" pin="PG"/>
<pinref part="R1" gate="G$1" pin="1"/>
<wire x1="104.14" y1="144.78" x2="111.76" y2="144.78" width="0.1524" layer="91"/>
</segment>
</net>
<net name="FB2" class="0">
<segment>
<pinref part="R2" gate="G$1" pin="1"/>
<pinref part="R3" gate="G$1" pin="2"/>
<wire x1="134.62" y1="139.7" x2="134.62" y2="137.16" width="0.1524" layer="91"/>
<pinref part="IC1" gate="G$1" pin="FB"/>
<wire x1="134.62" y1="137.16" x2="134.62" y2="134.62" width="0.1524" layer="91"/>
<wire x1="104.14" y1="137.16" x2="134.62" y2="137.16" width="0.1524" layer="91"/>
<junction x="134.62" y="137.16"/>
</segment>
</net>
<net name="SW2" class="0">
<segment>
<pinref part="IC1" gate="G$1" pin="SW"/>
<wire x1="104.14" y1="142.24" x2="106.68" y2="142.24" width="0.1524" layer="91"/>
<wire x1="106.68" y1="142.24" x2="106.68" y2="154.94" width="0.1524" layer="91"/>
<pinref part="L1" gate="G$1" pin="1"/>
</segment>
</net>
</nets>
</sheet>
</sheets>
</schematic>
</drawing>
<compatibility>
<note version="8.2" severity="warning">
Since Version 8.2, EAGLE supports online libraries. The ids
of those online libraries will not be understood (or retained)
with this version.
</note>
<note version="8.3" severity="warning">
Since Version 8.3, EAGLE supports URNs for individual library
assets (packages, symbols, and devices). The URNs of those assets
will not be understood (or retained) with this version.
</note>
<note version="8.3" severity="warning">
Since Version 8.3, EAGLE supports the association of 3D packages
with devices in libraries, schematics, and board files. Those 3D
packages will not be understood (or retained) with this version.
</note>
<note version="8.4" severity="warning">
Since Version 8.4, EAGLE supports properties for SPICE simulation. 
Probes in schematics and SPICE mapping objects found in parts and library devices
will not be understood with this version. Update EAGLE to the latest version
for full support of SPICE simulation. 
</note>
</compatibility>
</eagle>
