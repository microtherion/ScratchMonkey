function selectProtocol()
{
    rebuildTargets();
    selectProgrammer();
    selectTarget();
}

function selectProgrammer()
{
    var proto = document.connectionOptions.protocol.value;
    var prog  = document.connectionOptions.programmer.value;
    programmerDiagram = document.getElementById("programmerDiagram")
    if (proto != "none" && prog != "none") {
	programmerDiagram.data = "img/" + prog + "_prog_" + proto + ".svg";
	programmerDiagram.style.display = "inline";
    } else {
	programmerDiagram.style.display = "none";
    }
}

var gProto = "none";
var gTarget= {
  "none": [
  ],
  "isp": [
    ["Arduino Uno", "uno"],
    ["Arduino Leonardo / Micro", "micro"],
    ["Arduino Nano", "nano"],
    ["ICSP Header", "icsp"],
    ["ATtiny85", "attiny85"],
    ["ATtiny84", "attiny84"],
    ["ATtiny861", "attiny861"],
    ["ATtiny4313", "attiny4313"],
    ["ATtiny1634", "attiny1634"],
    ["ATmega328", "atmega328"],
    ["ATmega1284", "atmega1284"],
  ],
  "hvsp": [
    ["Digispark", "digispark"],
    ["ATtiny85", "attiny85"],
    ["ATtiny84", "attiny84"],
  ],
  "hvpp": [
    ["ATtiny861", "attiny861"],
    ["ATtiny4313", "attiny4313"],
    ["ATtiny1634", "attiny1634"],
    ["ATmega328", "atmega328"],
    ["ATmega1284", "atmega1284"],
  ],
}

function rebuildTargets()
{
    var target= document.connectionOptions.target;
    var targ  = document.connectionOptions.target.value;
    var proto = document.connectionOptions.protocol.value;
    var select= 0;
    console.log(targ);
    if (proto != gProto) {
	target.options.length= 1;
	gTarget[proto].map(function(t) {
	    if (t[1] == targ) {
		select = target.options.length;
	    }
	    target.options[target.options.length] = new Option(t[0], t[1]);
	});
	target.selectedIndex = select;
	gProto = proto;
    }
}

function selectTarget()
{
    var proto = document.connectionOptions.protocol.value;
    var targ  = document.connectionOptions.target.value;
    Array.prototype.filter.call(document.getElementsByClassName("note"), function(elt) {
	elt.className = "note_hidden";
    });
    targetDiagram = document.getElementById("targetDiagram")
    if (targ != "none") {
	targetDiagram.data = "img/" + targ + "_" + proto + ".svg";
	targetDiagram.style.display = "inline";
	Array.prototype.filter.call(document.getElementsByClassName("note_hidden"), function(elt) {
	    if (elt.id == targ) {
		elt.className = "note";
	    }
	});
    } else {
	targetDiagram.style.display = "none";
    }
}
