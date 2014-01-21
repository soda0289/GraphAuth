const Clutter = imports.gi.Clutter;
const Lang = imports.lang;

const GraphAuth = imports.gdm.graphAuth;
const GraphAuthPam = imports.gi.GraphAuth;

function begin_auth(pam_auth){
            let err = 0;
            let status = pam_auth.authenticate(err);
            if(status[0] == 1){
                text.set_text("Authenticated!");
            }else{
                text.set_text("Authentication Failed. Error code" + status.toString());
           } 
}

function new_auth_message(){
    this.show();
    
    Clutter.main();

    return ga.key.toString();
}

function setup_stage(width, height){
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
    stage.set_size(width, height);
    stage.connect("destroy", Clutter.main_quit);

    return stage;
}

//Setup window that contaings Clutter stage
let stage = setup_stage(620,660);
stage.show();

//New graphAuth actor used to ask user for code
let ga = new GraphAuth.GraphAuth();
ga.connect("next", Clutter.main_quit);
ga.actor.hide();
stage.add_child(ga.get_actor());

let text = new Clutter.Text();

//PAM authentication gobject
let pam_auth = new GraphAuthPam.Pam();
//new-message signal is PAM conversation
//we show the graphAuth actor and wait for user to swipe code.
pam_auth.connect("new-message", Lang.bind(ga, new_auth_message));

begin_auth(pam_auth);

