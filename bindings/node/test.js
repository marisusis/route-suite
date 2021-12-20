const APIClient = require("./lib/binding.js");

let clients = [];

const count = 4;
for (let i = 0; i < count; i++) {

    // create a client
    clients[i] = new APIClient("TestClientJS" + i);


    // open client
    clients[i].open();

    // get the refnum
    console.log(`client ${i} ref=${clients[i].getRef()}`);

}

process.on('SIGINT', function() {

    for (let i = 0; i < count; i++) {

        // close client
        clients[i].close();

    }

    process.exit();
});


// sleep
const sleepTime = 10000;
console.log(`sleeping for ${sleepTime}ms...`);

for (let i = 0; i < count; i++) {

    // close client
    console.log(`client ${i} ref ${clients[i].getRef()}`);

}

setTimeout(function(){
    for (let i = 0; i < count; i++) {

        // close client
        clients[i].close();

    }
},sleepTime);



