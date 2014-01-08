const GraphAuthIntro = imports.gi.GraphAuth;


function _new_auth_message(event, msg){
        print("MSG: " + msg);
        return "hehehehe";
}

let pam_auth = new GraphAuthIntro.Pam();
let error = 1;
let key = "testtesttest"
pam_auth.connect("new-message", _new_auth_message);
pam_auth.authenticate(key ,error);
