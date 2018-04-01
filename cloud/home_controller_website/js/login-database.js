$(document).ready( function() {

  $("#submit-btn").click(function(){
      $("#inputUsername").prop('required',true);
      $("#inputPassword").prop('required',true);

      // Register
      if ($("#Register").hasClass('active')){
        var username = $('#inputUsername').val();
        var password = $('#inputPassword').val();

        // var form = $("#pictureInput");
        // var formdata = false;
        // if (window.FormData){
        //     formdata = new FormData(form[0]);
        // }

        var canvas = document.getElementById('canvas');
        var context = canvas.getContext('2d');

        var imgData = context.getImageData(0, 0, 640, 480).data;
        console.log(imgData)

        var form = new FormData();
        form.append("username", username);
        form.append("password", password);
        form.append("picture", imgData);
        form.append("width", 640)
        form.append("height", 480)

        var settings = {
          "async": true,
          "crossDomain": true,
          "url": "http://104.198.97.189:6000/joinByRGB",
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
        $("#inputUsername").removeAttr('required');
        $("#inputPassword").removeAttr('required');

        // var form = $("#pictureInput");
        // var formdata = false;
        // if (window.FormData){
        //     formdata = new FormData(form[0]);
        // }

        var canvas = document.getElementById('canvas');
        var context = canvas.getContext('2d');

        var imgData = context.getImageData(0, 0, 640, 480).data;

        var form = new FormData();
        form.append("picture", imgData);
        form.append("width", 640)
        form.append("height", 480)

        var settings = {
          "async": true,
          "crossDomain": true,
          "url": "http://104.198.97.189:6000/loginByRGB",
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
  });

  $("#LoginByInfo").click(function(){
    console.log("clicked");
    $("#inputUsername").prop('disabled', false);
    $("#inputPassword").prop('disabled', false);
  });

  $("#Register").click(function(){
    console.log("clicked");
    $("#inputUsername").prop('disabled', false);
    $("#inputPassword").prop('disabled', false);
  });

  function load_user_username(settings, username) {
    window.loading_screen = window.pleaseWait({
      logo: "photos/logo-mini.png",
      backgroundColor: '#262626',
      loadingHtml: "<p class='loading-message' style='color: #dbdbdb'>Please Wait</p><div class='spinner'><div class='rect1'></div><div class='rect2'></div><div class='rect3'></div><div class='rect4'></div><div class='rect5'></div></div>"
    });

    $.ajax(settings).done(function (response) {
      if (response == username){


        window.loading_screen.finish();

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
      else if (response == "Username Taken"){
        window.loading_screen.finish();
        $("#errorMessage").text("Username Taken!");
        $("#inputUsername").val("");
        $("#inputPassword").val("");
      }
      else {
        window.loading_screen.finish();
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
