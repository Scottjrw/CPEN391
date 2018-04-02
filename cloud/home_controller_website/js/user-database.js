$(document).ready( function() {

  // filling the ifttt div
  var mapping_count = 0;

  getCurrentUser();
  updateIFTTT();
  getCurrentMapping();

  function updateIFTTT() {
    var settings = {
      "async": true,
      "crossDomain": true,
      "url": "http://104.198.97.189:6000/getUserApplets",
      "method": "GET",
      "processData": false,
      "crossDomain": true,
      "contentType": false
    }
    $.ajax(settings).done(function (response) {
      console.log(response);
      var list_of_applets = "";

      for (var i = 0; i < response.length; i++) {
        list_of_applets = list_of_applets + "<a id='" + i + "' draggable='true' ondragstart='drag(event)''>" + response[i] + "</a><br />";
      }

      document.getElementById("action-list").innerHTML = list_of_applets;

    });
  }

  function getCurrentUser() {
    var settings = {
      "async": true,
      "crossDomain": true,
      "url": "http://104.198.97.189:6000/getCurrentUser",
      "method": "GET",
      "processData": false,
      "crossDomain": true,
      "contentType": false
    }
    $.ajax(settings).done(function (response) {
      document.getElementById("welcome-message").innerHTML = "Welcome, " + response;
    });
  }

  function addMappingDiv() {
    mapping_count++;
    document.getElementById("droppable-gestures").innerHTML += "<div id='gesture" + mapping_count +  "' class='droppable' ondrop='drop(event)' ondragover='allowDrop(event)'></div>";
    document.getElementById("droppable-actions").innerHTML += "<div id='action" + mapping_count +  "' class='droppable' ondrop='drop(event)' ondragover='allowDrop(event)'></div>";
  }

  function removeMappingDiv() {
    mapping_count--;
    document.getElementById("droppable-gestures").removeChild(document.getElementById("droppable-gestures").lastChild);
    document.getElementById("droppable-actions").removeChild(document.getElementById("droppable-actions").lastChild);
  }

  function getCurrentMapping() {
    var settings = {
      "async": true,
      "crossDomain": true,
      "url": "http://104.198.97.189:6000/getCurrentMapping",
      "method": "GET",
      "processData": false,
      "crossDomain": true,
      "contentType": false
    }
    $.ajax(settings).done(function (response) {
      console.log(response);

      if (response.length == 0){
        console.log("empty");
        addMappingDiv();
        addMappingDiv();
      }

      for (var i = 0; i < response.length; i++) {
        addMappingDiv();
        var gesture_string = "gesture" + (i+1).toString();
        var action_string = "action" + (i+1).toString();
        document.getElementById(gesture_string).innerHTML = "<a id='" + response[i][0] + "' draggable='true' ondragstart='drag(event)''>" + response[i][0] + "</a><br />";
        document.getElementById(action_string).innerHTML = "<a id='" + response[i][1] + "' draggable='true' ondragstart='drag(event)''>" + response[i][1] + "</a><br />";
      }

    });
  }

  function changeCurrentMapping() {
    var gestures = [];
    var actions = [];
    var mapping = [];

    $(".droppable-gestures div").each(function( index ) {
      if ($(this).children().text() == ""){
        return;
      }
      gestures.push($(this).children().text());
    });
    $(".droppable-actions div").each(function( index ) {
      if ($(this).children().text() == ""){
        return;
      }
      actions.push($(this).children().text());
    });

    // add errors here
    if (gestures.length != actions.length) {
      return "Error";
    }

    for (var i = 0; i < gestures.length; i++) {
      mapping.push([gestures[i], actions[i]]);
    }

    return mapping;
  }

  $("#faq").click(function() {
    console.log("ok");
    $("#modal-title").text('Frequently Asked Questions');
    $("#modal-body").html("<p>Welcome to your account!  Below you may add your specialized 'IF THIS, THEN THAT' Services to be mapped to guestures to use on your Home Controller. For more information on how to make IFTT Services and how to get the URLs see here: </p><a href='https://ifttt.com/discover' target='_blank'>IFTTT Webpage Home</a> //<a href='https://help.ifttt.com/hc/en-us/articles/115010361388-How-do-I-create-an-Applet-' target='_blank'>How to Create a Service</a>");
    $("#myModal").modal('toggle');
  });

  $("#logout").click(function() {
    var settings = {
      "async": true,
      "crossDomain": true,
      "url": "http://104.198.97.189:6000/logout",
      "method": "GET",
      "processData": false,
      "crossDomain": true,
      "contentType": false
    }
    $.ajax(settings).done(function (response) {
    });

    window.loading_screen = window.pleaseWait({
      logo: "../photos/logo-mini.png",
      backgroundColor: '#262626',
      loadingHtml: "<p class='loading-message' style='color: #dbdbdb;'>Logging Out</p><div class='spinner'><div class='rect1'></div><div class='rect2'></div><div class='rect3'></div><div class='rect4'></div><div class='rect5'></div></div>"
    });

    console.log("delay");

    setTimeout(function(){

       //whatever the heck you want do to:

       window.loading_screen.finish();

       window.location = ("../index.html");

    }, 5000); //7.5 seconds

  });

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
        $("#modal-body").text('Added ' + descriptor + ' at URL: ' + url);
        updateIFTTT();
        $("#descriptor").text('');
        $("#url").text('');
      }
      else if (response == "Service Exists"){
        $("#modal-title").text('Uh Oh!');
        $("#modal-body").text( descriptor + ' already exits...');
      }
      $("#myModal").modal('toggle');
      $("#descriptor").val('');
      $("#url").val('');
    });

  });

  $("#add-btn").click(function() {
    addMappingDiv();
  });

  $("#minus-btn").click(function() {
    removeMappingDiv();
  });

  $("#submit-btn").click(function() {
    var mapping = changeCurrentMapping();
    if (mapping == "Error"){
      $("#modal-title").text('Error!');
      $("#modal-body").text('The amount of Gestures do not match the number of Actions.');
      $("#myModal").modal('toggle');
      return;
    }
    var form = new FormData();

    form.append("mapping", mapping);

    var settings = {
      "async": true,
      "crossDomain": true,
      "url": "http://104.198.97.189:6000/changeCurrentMapping",
      "method": "POST",
      "processData": false,
      "crossDomain": true,
      "contentType": false,
      "mimeType": "multipart/form-data",
      "data": form
    }
    $.ajax(settings).done(function (response) {
      console.log(response);
      if (response == "Success"){
        $("#modal-title").text('Success!');
        $("#modal-body").text('You have changed your Gesture Map');
      }
      $("#myModal").modal('toggle');
    });
  });

});
