  process.stdin.resume();
  process.stdin.setEncoding('utf8');
  var util = require('util');

  process.stdin.on('data', function (text) {
    console.log('received data:', util.inspect(text));
    if (text === 'quit\n') {
      done();
    }
    if (text === 'q\n') {
      done();
    }
   
//colorChecker
    if (text === 'r\n') {
      r++;
    }
    if (text === 'g\n') {
      g++;
    }
    if (text === 'b\n') {
      b++;
    }

  });

  function done() {
    console.log('There is nothing more to do.');
    quitting = true;
    // process.exit();
  }
