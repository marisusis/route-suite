const addon = require('../build/Release/hello');

function APIClient(name) {

    this.open = function() {
        _addonInstance.open();
    }

    this.close = function() {
        _addonInstance.close();
    }

    this.getRef = function() {
        return _addonInstance.getRef();
    }

    let _addonInstance = new addon.APIClientWrap(name);

}

module.exports = APIClient;