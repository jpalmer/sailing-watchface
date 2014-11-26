function LOG(l) {console.log(JSON.stringify(l))}
var maxspeed = -1;
var sendmessage_success=function(e){LOG("successfully sent with ID="+e.data.transactionID);};

var sendmessage_failure=function(e){LOG("Failed to send message, error" + e.error.message);};
function sendspeed(speed,heading,max)       {Pebble.sendAppMessage({"0":(""+speed),"1":(""+max),"2":(""+heading)},sendmessage_success,sendmessage_failure);}
var maxstr = "---";
function gotpos(p)
{
    var speedstr="";
    var headstr="";
    if (p.coords.speed == null)
    {
        speedstr="----";
        headstr="---";
    }
    else
    {
        var speed = p.coords.speed * 1.94384;
        if (speed>maxspeed)
        {
            maxspeed=speed;
            maxstr=(maxspeed.toFixed(1));
        }
        speedstr=speed.toFixed(1);
        if (p.coords.heading !== null)
        {
            headstr=p.coords.heading.toFixed(1);
        }
    }
    LOG("sending speed" + speedstr + "heading" + headstr + "max" + maxstr);
    sendspeed(speedstr,headstr,maxstr);
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
