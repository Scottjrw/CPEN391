$(document).ready( function() {

  $("#submit-btn").click(function(){
      $("#pictureInput").prop('required',true);
      $("#inputUsername").prop('required',true);
      $("#inputPassword").prop('required',true);

      var text = $('#dtoggle').html();
      // Register
      if ($("#Register").hasClass('active')){
        var username = $('#inputUsername').val();
        var password = $('#inputPassword').val();

        var form = $("#pictureInput");
        var formdata = false;
        if (window.FormData){
            formdata = new FormData(form[0]);
        }

        var form = new FormData();
        form.append("username", username);
        form.append("password", password);
        form.append("file", $('input[type=file]')[0].files[0]);

        var settings = {
          "async": true,
          "crossDomain": true,
          "url": "http://104.198.97.189:6000/joinByPicture",
          "method": "POST",
          "processData": false,
          "crossDomain": true,
          "contentType": false,
          "mimeType": "multipart/form-data",
          "data": form
        }

        load_user_username(settings, username);
      }
      // Login
      else if ($("#LoginByPicture").hasClass('active')){
        $("#pictureInput").prop('required',true);
        $("#inputUsername").removeAttr('required');
        $("#inputPassword").removeAttr('required');

        var form = $("#pictureInput");
        var formdata = false;
        if (window.FormData){
            formdata = new FormData(form[0]);
        }

        var form = new FormData();
        form.append("file", $('input[type=file]')[0].files[0]);

        var settings = {
          "async": true,
          "crossDomain": true,
          "url": "http://104.198.97.189:6000/loginByFacePicture",
          "method": "POST",
          "processData": false,
          "crossDomain": true,
          "contentType": false,
          "mimeType": "multipart/form-data",
          "data": form
        }

        load_user_picture(settings);
      }
      else {
        $("#inputUsername").prop('required',true);
        $("#inputPassword").prop('required',true);
        $("#pictureInput").removeAttr('required');

        var username = $('#inputUsername').val();
        var password = $('#inputPassword').val();

        var form = new FormData();
        form.append("username", username);
        form.append("password", password);

        var settings = {
          "async": true,
          "crossDomain": true,
          "url": "http://104.198.97.189:6000/loginByPasswordWebsite",
          "method": "POST",
          "processData": false,
          "crossDomain": true,
          "contentType": false,
          "mimeType": "multipart/form-data",
          "data": form
        }

        load_user_username(settings, username);

      }

  });


  $("#LoginByPicture").click(function(){
    console.log("clicked");
    $("#inputUsername").prop('disabled', true);
    $("#inputPassword").prop('disabled', true);
    $("#pictureInput").prop('disabled', false);
  });

  $("#LoginByInfo").click(function(){
    console.log("clicked");
    $("#inputUsername").prop('disabled', false);
    $("#inputPassword").prop('disabled', false);
    $("#pictureInput").prop('disabled', true);
  });

  $("#Register").click(function(){
    console.log("clicked");
    $("#inputUsername").prop('disabled', false);
    $("#inputPassword").prop('disabled', false);
    $("#pictureInput").prop('disabled', false);
  });

  function load_user_username(settings, username) {
    $.ajax(settings).done(function (response) {
      if (response == username){

        window.loading_screen = window.pleaseWait({
          logo: "photos/logo-mini.png",
          backgroundColor: '#262626',
          loadingHtml: "<p class='loading-message' style='color: #dbdbdb'>Welcome " + username + "!</p><div class='spinner'><div class='rect1'></div><div class='rect2'></div><div class='rect3'></div><div class='rect4'></div><div class='rect5'></div></div>"
        });

        console.log("delay");

        setTimeout(function(){

           //whatever the heck you want do to:

           window.loading_screen.finish();

           window.location = ("user/index.html");

        }, 5000); //7.5 seconds

      }
      else {
        $("#errorMessage").text("Incorrect Credentials!");
        $("#inputUsername").val("");
        $("#inputPassword").val("");
      }
    });
  }

  function load_user_picture(settings) {
    $.ajax(settings).done(function (response) {
      if (response != "login failed"){

        window.loading_screen = window.pleaseWait({
          logo: "photos/logo-mini.png",
          backgroundColor: '#262626',
          loadingHtml: "<p class='loading-message' style='color: #dbdbdb;'>Welcome " + response + "!</p><div class='spinner'><div class='rect1'></div><div class='rect2'></div><div class='rect3'></div><div class='rect4'></div><div class='rect5'></div></div>"
        });

        console.log("delay");

        setTimeout(function(){

           //whatever the heck you want do to:

           window.loading_screen.finish();

           window.location = ("user/index.html");

        }, 5000); //7.5 seconds

      }
      else {
        $("#errorMessage").text("No Matching Users!");
      }
    });
  }

});
