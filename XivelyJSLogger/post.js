var request = require('request');


/**** READ CSV FILE ******/

if (process.argv.length < 3) {
  console.log('Usage: node ' + process.argv[1] + ' FILENAME');
  process.exit(1);
}

// Read the file and print its contents.
var fs = require('fs'), filename = process.argv[2];


var locationData = [];
var dataPoints = []

var file = fs.readFileSync(filename,"utf8");
var d= file.split("\n");
    for (var i = 0; i < d.length; i++) {
            
            var params = d[i].split(",");
            console.log(params);
            var dp = {      "at":params[0],
                            "value":params[1]
                        } ;
            var location = {
                    "location":{
                        "disposition":"mobile",
                        "lat":params[2],
                        "long":params[3]}
                    };
            locationData.push(location);
            dataPoints.push(dp);
    };
// 
// console.log(locationData);
// console.log(dataPoints);


/**** ADD XIVELY API INFORMATION ******/
var FEED_ID="1262507360";
var API_KEY = 'DrjCwzDUpfRaEkwZ8wL5d4VhedAURcCsckQir3Iwys6tAYoI';


/**** POPULATE HTTP HEADERS ******/
var headers = { "X-ApiKey" : API_KEY,
                "Content-Type": "application/json"}   

var settings1 ={
                "location": {
                    "disposition": "mobile",
                    "name": "New York City",
                    "exposure": "outdoor",
                    "domain": "physical",
                    "lat": 40.712784,
                    "lon": -74.005941
                    }
                }
var postDataObj =  {
                  "version":"1.0.0",
                   "datastreams" : [ {
                        "id" : "sensor_value",
                        "datapoints":dataPoints,
                        "current_value" : "50"
                    }
                  ],
                }

/**** POPULATE DATA SPECIFIC HEADERS ******/
var postLocationOptions = {
    url: 'https://api.xively.com/v2/feeds/'+FEED_ID+'.json',
    qs: {'api_key': API_KEY},
    method: 'PUT',
    headers: headers,
    body :JSON.stringify(locationData[0])
}


/**** POPULATE LOCATION SPECFIC HEADERS ******/
var postDataOptions = {
    url: 'https://api.xively.com/v2/feeds/'+FEED_ID+'.json',
    qs: {'api_key': API_KEY},
    method: 'PUT',
    // headers: headers,
    body :JSON.stringify(postDataObj)
}
console.log(JSON.stringify(postDataObj));
/**** POST DATA INFORMATION ******/
request(postDataOptions, function (error, response, body) {
    if (!error && response.statusCode == 200) {
        // Print out the response body
        // console.log(body)
    }
    else{
     // console.log(response.statusCode)   
     // console.log(body)   
    }
});

/**** POST LOCATION INFORMATION ******/
request(postLocationOptions, function (error, response, body) {
    if (!error && response.statusCode == 200) {
        // Print out the response body
        // console.log(body)
    }
    else{
     // console.log(response.statusCode)   
     // console.log(body)   
    }
});

