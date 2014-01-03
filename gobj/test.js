const GraphAuthIntro = imports.gi.GraphAuth;

let pam_auth = new GraphAuthIntro.Pam();
let error = 1;
let key = "testtesttest"
pam_auth.authenticate(key ,error);
