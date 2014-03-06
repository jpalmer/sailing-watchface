function LOG(l) {console.log(JSON.stringify(l))}
var maxspeed = -1;
var sendmessage_success=function(e){LOG("successfully sent with ID="+e.data.transactionID);};

var sendmessage_failure=function(e){LOG("Failed to send message, error" + e.error.message);};
function sendspeed(speed) {Pebble.sendAppMessage({"0":(""+speed)},sendmessage_success,sendmessage_failure);}
function gotpos(p)
{
    var speedstr="";
    if (p.coords.speed == null)
    {
        speedstr="----";
    }
    else
    {
        var speed = p.coords.speed * 1.94384;
        if (speed>maxspeed)
        {
           // LOG("sending maxspeed");
            maxspeed=speed;
            //send("1",""+maxspeed.toFixed(2));
        }
        speedstr=speed.toFixed(1);
    }
    LOG("sending speed" + speedstr);
    sendspeed(speedstr);
}
function handle_error(err) {console.log("error is" + err);}
Pebble.addEventListener("ready",function()
    {
        console.log("js started");
        var gl = navigator.geolocation;
        console.log("got geoloc");
        setInterval(
              function(){  
                  var pos = gl.getCurrentPosition(gotpos,handle_error,{enableHighAccuracy:true});
              },5000);
        console.log("ready done");
    });
