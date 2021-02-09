const express = require("express");

const app = express();
const port = process.env.PORT || 3000;
var crypto = require("crypto");
var bodyParser = require('body-parser')
app.use(bodyParser.urlencoded());
// app.use(bodyParser.json());


let lock = false;
let token = undefined;
let timeout = 40; //seconds
let time = Date.now();
let code_length = 8;
let macs = ['00:90:4C:C9:AB:EF','70:8A:09:73:7A:E6','80:CE:B9:4B:B4:15']
generateCode = function(length){
  var id = crypto.randomBytes(length/2).toString('hex');
  return id
}

app.get("/get_code", (req, res) => {
  token = generateCode(code_length);
  lock = false;
  time = Date.now();
  res.send(token);
});

app.get("/get_lock", (req, res) => {
  res.send(lock.toString());
});

app.post("/check_code", (req, res) => {
  let a = Date.now() - time
  if(a > timeout*1000){
    res.send("timeout " + a.toString());
    return
  }
  if(token.toString().trim() === req.body.token.toString().trim()){
    if (macs.includes(req.body.mac)){
      lock =true;
      
      setTimeout(()=>{
        lock = false;
      }, 10000);

      res.send("opened");
      return
    }
    res.send("device not valid mac: "+ req.body.mac)
    return
  }


  res.send("token not valid: "+ req.body.token + " " + token);
});

app.get("/", (req, res) => {
  res.send("Welcome");
});

app.listen(port, () => {
  console.log(`Example app listening at http://localhost:${port}`);
});
