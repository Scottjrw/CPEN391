$(document).ready( function() {

  $("#btn-addIFTTT").click(function(){
    var descriptor = $("#descriptor").val();
    var url = $("#url").val();

    console.log(descriptor);
    console.log(url);

    var form = new FormData();
    form.append("descriptor", descriptor);
    form.append("url", url);

    var settings = {
      "async": true,
      "crossDomain": true,
      "url": "http://104.198.97.189:6000/addApplet",
      "method": "POST",
      "processData": false,
      "crossDomain": true,
      "contentType": false,
      "mimeType": "multipart/form-data",
      "data": form
    }

    $.ajax(settings).done(function (response) {
      if (response == "Success"){
        $("#modal-title").text('Success!');
        $("#modal-body").text('Added ' + descriptor + 'at URL: ' + url);

      }
      else if (response == "Service Exists"){
        $("#modal-title").text('Uh Oh!');
        $("#modal-body").text( descriptor + ' already exits...');
      }
      $("#myModal").modal('toggle');
    });

  });
});
