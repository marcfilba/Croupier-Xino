
$(document).ready(function(){
    //connect to the socket server.
    var socket = io.connect('http://' + document.domain + ':' + location.port + '/test');
    var ronda = [];
    var data_received = [];
    var teams = ["Hold the Door", "Team Rocket","Los Petadores", "Los Angeles de Llaberia", "Blob", "La Banca"]
    var punts = [0,0,0]

    //receive details from server
    socket.on('newMsg', function(msg) {
        d = msg.data.split (",")
        var players = msg.players

        data_received.unshift (msg.data);
        ronda.unshift (ronda.length + 1);

        data_string = "<ul><li><div class=\"headSizes leftSize\">&nbsp;</div>"
        for (var i = 0; i < players; i++){
          punts [i] += parseInt(d[i]);
          data_string += "<div class=\"headSizes\"> " + teams[i] + " (" + punts[i] + ") </div>"
        }
        data_string += "</li></ul>"
        data_string += "<ul class=\"locked\">"

        for (var i = 0; i < ronda.length; i++){
          data_string += "<li> <div class=\"leftSize\">Ronda " + ronda [i] + "</div>"
          for (var j = 0; j < players; j++){
            data_string += '<div class="bodySizes">' + d[j] + '</div>'
          }
          data_string += '</li>'
        }
        $('#log').html(data_string);
    });

});
