
$(document).ready(function(){
    //connect to the socket server.
    var socket = io.connect('http://' + document.domain + ':' + location.port + '/test');
    var ronda = 0;
    var data_received = [][];

    //receive details from server
    socket.on('newMsg', function(msg) {
        console.log("Received data" + msg.data);
        data_received.unshift(msg.data);
        data_string = '';
        for (var i = 0; i < data_received.length; i++){
          data_string = data_string + '<li> <div class="leftSize">Ronda ' + ronda.toString() + '</div>'
          for (var j = 0; j < msg.players; j++){
            data_string = data_string + '<div class="bodySizes">' + data_received[j].toString() + '</div>'
          }
          data_string = data_string + '</li>'
        }
        $('#log').html(data_string);
    });

});
