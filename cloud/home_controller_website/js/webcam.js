// Grab elements, create settings, etc.
var video = document.getElementById('webcam');

// Get access to the camera!
if(navigator.mediaDevices && navigator.mediaDevices.getUserMedia) {
    // Not adding `{ audio: true }` since we only want video now
    navigator.mediaDevices.getUserMedia({ video: true }).then(function(stream) {
        video.src = window.URL.createObjectURL(stream);
        video.play();
    });
}

// Elements for taking the snapshot
var canvas = document.getElementById('canvas');
var context = canvas.getContext('2d');

// Trigger photo take
document.getElementById("snap").addEventListener("click", function() {

  var new_width = video.offsetWidth + "px";
  var new_height = video.offsetHeight + "px";
  console.log(new_width)
  console.log(new_height)


	context.drawImage(video, 0, 0, 640, 480);
  document.getElementById("webcam").style.zIndex = "-1";
  document.getElementById("canvas").style.zIndex = "100";
  document.getElementById("canvas").style.width = new_width;
  document.getElementById("canvas").style.height = new_height;

  var link = document.createElement('a');
  link.download = "Drag-Drop-To-Upload.png";
  link.href = canvas.toDataURL("image/png").replace("image/png", "image/octet-stream");;
  link.click();
});
