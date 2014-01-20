const Clutter = imports.gi.Clutter;
imports.searchPath.unshift("./");
const GraphAuth = imports.graphAuth;

function on_next(){

            status = pam_auth.authenticate(this.key.toString());
            if(status[0] == 1){
                text.set_text("Authenticated!");
            }else{
                text.set_text("Authentication Failed. Error code" + status.toString());
           } 
}

Clutter.init(null, 0);

let stage = new Clutter.Stage();
let color = new Clutter.Color({
        red : 150,
        green : 150,
        blue : 150,
    alpha: 255
});
stage.set_background_color(color);
stage.title = "graphAuth";
stage.set_size(520,550);
stage.connect("destroy", Clutter.main_quit);

//PAM authentication gobject
//new-message signal is PAM conversation
pam_auth = new GraphAuthIntro.Pam();
pam_auth.connect("new-message", Lang.bind(this, this._new_auth_message));

let ga = new GraphAuth.graphAuth();
stage.add_child(ga.get_actor());

ga.show();
stage.show();
Clutter.main();
