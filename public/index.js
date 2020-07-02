var tgl = document.getElementById("tanggal_id");
var jamke = document.getElementById("in_jamke");
var menitke = document.getElementById("in_menitke");
var today = new Date();
var dd = today.getDate();
var mm = today.getMonth()+1; //January is 0!
var yyyy = today.getFullYear();
var hh = today.getHours();
var MM = today.getMinutes();
var dataa = 0;
// if(dd<10) {
    // dd = '0'+dd
// } 

// if(mm<10) {
    // mm = '0'+mm
// } 

today = dd + '-' + mm + '-' + yyyy;
// document.write(today);
//tanggal inputan

tgl.value = today;
jamke.value = ("jamke"+hh);
menitke.value = ("menitke"+MM);
ambill_data2();
var nomor = 0;
function ambill_data2(){
	window.alert(today + hh + MM);
	nomor = 0;
	// window.alert(nomor);
	var dir = today + "/" + hh + "/" + MM ;
	// window.alert(dir);
	var testing =  firebase.database().ref(dir);
	testing.once('value' ,function(datasnapshot){
		// mengambil data banyak dengan metode ForEach
			datasnapshot.forEach(function(child) {
				
				// window.alert(child.key+child.val());
				var table = document.getElementById("myTable");
				var row = table.insertRow(0);
				var cell1 = row.insertCell(0);
				var cell2 = row.insertCell(1);
				// if(nomor == 0 ){
					// cell1.innerHTML = "Detik";
					// cell2.innerHTML = "Richter Scale";
					// nomor = nomor + 1;
				// }
				// else{
					cell1.innerHTML = child.key;
					cell2.innerHTML = child.val();
					nomor = nomor + 1;
					
				// }
				// window.alert(nomor);
				// striing = child.key+": "+child.val();
				// aray1[nomor+1] = child.key+": "+child.val();
				// aray1[nomor+1] = child.val();
				// window.alert(aray1[nomor+1]);
				
				
			});
			// /*if(aray1[0] == undefined){
			// window.alert(aray1[0] + " ini dia");
			// window.alert(aray1[3] + " ini dia");
			// }*/
		});
} 

function submit() {
	for(var a = 0 ; a<nomor; a++){
  // var table = document.getElementById("myTable");
  // var row = table.insertRow(0);
  // var cell1 = row.insertCell(0);
  // var cell2 = row.insertCell(1);
  // cell1.innerHTML = "NEW CELL1";
	// cell2.innerHTML = "NEW CELL2";
	document.getElementById("myTable").deleteRow(0);
	}
	
	today  =  tgl.value;
	var textjam= jamke.options[jamke.selectedIndex].text;
	var textmenit= menitke.options[menitke.selectedIndex].text;
	// window.alert(textjam + "    "+textmenit);
	hh = textjam;
	MM = textmenit;
	ambill_data2();
}