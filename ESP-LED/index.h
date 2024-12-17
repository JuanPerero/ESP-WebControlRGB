const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html> <head> <title> Control RGB en ESP </title>
<meta http-equiv="expires" content="0">
<meta http-equiv="Cache-Control" content="no-cache"> 
<meta http-equiv="Pragma" CONTENT="no-cache">
<style type="text/css">

section.Toques{    /* EL BOTON */
    align-content: center;
    top: 20px;
    background-color:#747474; 
    border: 1px black solid;
    border-top: 4px solid  #b3b3b3;
    border-left: 4px solid  #b3b3b3;
    border-bottom: 4px solid  #5e5e5e;
    border-right: 4px solid  #5e5e5e;
    width: 100px;
    text-align: center;
}

#main-header {
  background: rgb(46, 46, 46);
  color: white;
  height: 20%;
    padding: 10px;
} 

a.site-name{
  font-size: 10vh;
  font-weight:bolder;
  width:100%;
  color: rgb(99, 99, 99);
  position:relative;
  top:25%;
  left:10%;
}

a.site-desc {
    display: inline-block;
    font-weight: 300;
    font-size: 3vh;
    color:rgb(192, 209, 209);
    position: relative;
    margin-right: 4%;
}

a.barras{
  font-size: 4vh;
  position: relative;
  margin-right: 3%;
}

b{
  position: absolute;
  left: 10%;
  width: 20%;
  height:8%; 
  font-size:2vh; 
  background-color: rgb(99, 99, 99);
}

input.tono{
  position: absolute;
  left: 32%;
  width: 60%;
  height:10%; 
  font-size:100%; 
}




@media (orientation: portrait) {
  input.tono{
  font-size:100%; 
}
a.site-name{
  font-size: 8vw;
}

a.site-desc {
     font-size: 3vw;
 }

a.barras{
  font-size: 4vw;
}

b{
  font-size:2vw; 
}

}

</style> </head> 

<script>
function mandarRGB(){
  var valr = document.getElementById('rojo').value;
  var valg = document.getElementById('verde').value;
  var valb = document.getElementById('azul').value; 
  changeColor(valr,valg,valb);
  var texto = "Seteado "+valr+"-"+valg+"-"+valb;
  document.getElementById("resp").innerHTML = texto;
}
function panelRGB(){
  var h = document.getElementById("CL").value;
  let r = 0, g = 0, b = 0;
 if (h.length == 7) {
    r = Number("0x"+h[1] + h[2]);
    g = Number("0x"+h[3] + h[4]);
    b = Number("0x"+h[5] + h[6]);
  }
  document.getElementById('rojo').value = r;
  document.getElementById('verde').value = g;
  document.getElementById('azul').value = b;

  changeColor(r,g,b);
  var texto = "Seteado "+r+"-"+g+"-"+b;
  document.getElementById("resp").innerHTML = texto;
}
function changeColor(red, green, blue){
console.log(red,green,blue);
var xhttp = new XMLHttpRequest();
xhttp.onreadystatechange = function() {
if (this.readyState == 4 && this.status == 200) {
var txt = this.responseText;
}
};
xhttp.open("GET", "setRGB?r="+red+"&g="+green+"&b="+blue, true);
xhttp.send();
}
function veccolores(indice){
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
  if (this.readyState == 4 && this.status == 200) {
  var txt = this.responseText;
  }
};
xhttp.open("GET", "indx?ind="+indice, true);
xhttp.send();
}
function PalCol()
{
  var ColorHex =  document.getElementById("CL").value;
  function hexToRgb(hex) {
    var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
    return result ? {
      r: parseInt(result[1], 16),
      g: parseInt(result[2], 16),
      b: parseInt(result[3], 16)
  } : null;
}
  document.getElementById("rojo").value = hexToRgb(ColorHex).r;
  document.getElementById("verde").value = hexToRgb(ColorHex).g;
  document.getElementById("azul").value = hexToRgb(ColorHex).b;
}

function setbrillo(){
  var brillo = document.getElementById("I").value;  
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
     if (this.readyState == 4 && this.status == 200) {
      var txt = this.responseText;
     }
  };
  xhttp.open("GET", "setLUZ?I="+brillo, true);
  xhttp.send();
}

</script>





<body style="background-color:rgb(59, 59, 59);min-width: 480px;">
  
  <header id="main-header">
    <a class="site-name">Control RGB</a>
  </header>
    


  <section style="height: 10%;width: 80%; margin: auto;">
      <br>
      <a class="barras"> //</a>
    <a  href=/setFLASH class="site-desc"> Alternado</a> 
    <a class="barras"> //</a>
    <a  href=/setSLASH class="site-desc"> Variacion suave</a>
    <a class="barras"> //</a>
    <a  href=/setLUZ?I=10 class="site-desc">  Restablecer brillo</a> 
 </section>
 <section style="height: 10%;width: 80%; margin: auto;">
    <a class="barras"> //</a>
    <a  href=/flasheada class="site-desc"> Flashear gente</a> 
    <a class="barras"> //</a>
    <a href=/variado class="site-desc"> Mas colores </a>     
    <a class="barras"> //</a>
    <a href=/desvanecimiento class="site-desc"> desvanecimiento </a> 
  </section>
  <section style="height: 10%;width: 80%; margin: auto;">
      <a class="barras"> //</a>
    <a href=/wificonfig class="site-desc"> conf. conexion </a>
    <a class="barras"> //</a>
    <a href=/seeconfig class="site-desc"> Mostrar configuracion </a>
  </section>



<div style="height: 60%;width: 100%; position: absolute;">

  <div style="background-color:#474747; border:#5e5e5e 4px solid;height: 50%;width: 30%; padding: 2%; position: absolute; top: 5%; left:5%">


  <pre>
  <h2 style="position: absolute; font-size:3vh;background-color: rgb(99, 99, 99);left: 10%;top: 0%; width: 80%"> <font color="#aaaaaa">  RGB </font></h2>  
    <font color="#aaaaaa"> <b style="top: 20%;"> Tono: </b></font>
    <input class="tono" type="text" id="rojo" name="rojo" placeholder="Rojo" style="background-color:#ffcccc; top: 19%"> 
    <font color="#aaaaaa"> <b style="top: 35%;"> Tono: </b></font>
    <input class="tono" type="text" id="verde" name="verde" placeholder="Verde" style="background-color:#ccffcc;top: 34%"> 
    <font color="#aaaaaa"> <b style="top:50%"> Tono: </b></font>
    <input class="tono" type="text" id="azul" name="azul" placeholder="Azul" style="background-color:#ccccff;top: 49%">
    <input type="color" name="Color:" value="#ffffff" id="CL" style="position: absolute;top:65%; left:10%; background-color: #949494; width: 80%; height: 15%;" onchange="PalCol();"> 
  </pre>  


      <!--  Este es el boton     -->
    <!--section style="display: inline-block;"-->
      <section class="Toques" id="Inf0" style="position: absolute; top: 85%; left:10%;width: 60%;height: 7%; display: inline-block; font-size: 2vh;" onclick="panelRGB();" onmouseover="style.backgroundColor='#949494'" onmouseout="style.backgroundColor='#747474'"><font color="#333333">Mandar Color</font></section>
         <p id="resp" style="color: blanchedalmond;position: absolute; top: 100%; left:9%;width: 60%;height: 7%; display: inline-block;"> </p>
    <!--/section-->
  </div>



    <div id="colores" style=" background-color: #474747;border:#5e5e5e 4px solid; position: absolute; width: 20vh; height: 40vh; left: 50%; top: 3%">
      <input type="button" style="background-color: rgb(255, 0, 0); position: absolute; width: 24%; height: 12%; top: 5%; left: 10%;" onclick="veccolores(0)">
      <input type="button" style="background-color: rgb(0, 255, 0); position: absolute; width: 24%; height: 12%; top: 5%; left: 40%;" onclick="veccolores(5)">
      <input type="button" style="background-color: rgb(0, 0, 255); position: absolute; width: 24%; height: 12%; top: 5%; left: 70%;" onclick="veccolores(10)">

      <input type="button" style="background-color: rgb(255, 50, 0); position: absolute; width: 24%; height: 12%; top: 22%; left: 10%;" onclick="veccolores(1)">
      <input type="button" style="background-color: rgb(0, 255, 50); position: absolute; width: 24%; height: 12%; top: 22%; left: 40%;" onclick="veccolores(6)">
      <input type="button" style="background-color: rgb(50, 0, 255); position: absolute; width: 24%; height: 12%; top: 22%; left: 70%;" onclick="veccolores(11)">

      <input type="button" style="background-color: rgb(255, 125, 0); position: absolute; width: 24%; height: 12%;  top: 39%; left: 10%;" onclick="veccolores(2)">
      <input type="button" style="background-color: rgb(0, 255, 125); position: absolute; width: 24%; height: 12%;  top: 39%; left: 40%;" onclick="veccolores(7)">
      <input type="button" style="background-color: rgb(125, 0, 255); position: absolute; width: 24%; height: 12%;  top: 39%; left: 70%;" onclick="veccolores(12)">

      <input type="button" style="background-color: rgb(255, 180, 0); position: absolute; width: 24%; height: 12%;   top: 56%; left: 10%;" onclick="veccolores(3)">
      <input type="button" style="background-color: rgb(0, 255, 180); position: absolute; width: 24%; height: 12%;   top: 56%; left: 40%;" onclick="veccolores(8)">
      <input type="button" style="background-color: rgb(180, 0, 255); position: absolute; width: 24%; height: 12%;  top: 56%; left: 70%;" onclick="veccolores(13)">

      <input type="button" style="background-color: rgb(255, 255, 0); position: absolute; width: 24%; height: 12%;  top: 73%; left: 10%;" onclick="veccolores(4)">
      <input type="button" style="background-color: rgb(0, 255, 255); position: absolute; width: 24%; height: 12%;  top: 73%; left: 40%;" onclick="veccolores(9)">
      <input type="button" style="background-color: rgb(255, 0, 255); position: absolute; width: 24%; height: 12%;  top: 73%; left: 70%;" onclick="veccolores(14)">

      <input type="button" style="background-color: rgb(255, 255, 255); position: absolute; width: 35%; height: 10%; top: 89%; left: 10%;" onclick="veccolores(15)">
      <input type="button" style="background-color: rgb(0, 0, 0); position: absolute; width: 35%; height: 10%; color: rgb(255,255,255); top: 89%; left: 55%; font-size: 2vh" onclick="veccolores(16)" value="OFF">

    </div>
    
    <div id="inten" style="position: absolute; width: 40%; height: 10%; left: 5%; top: 70%">
    <a class="site-desc" style="position: absolute; left:5% ;top: 20%"> Intensidad </a>
    <input id="I" name="I" type="range" min="0" max="10" step="1" style="position: absolute;width: 60%;height: 20%;left: 40%;top: 35%;" onchange="setbrillo(this.value)">
    </div>


</div>

</body>
</html>
)=====";
