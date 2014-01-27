const Clutter = imports.gi.Clutter;
const Lang = imports.lang;

const GraphAuthPrompt = imports.graphAuthPrompt;

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
    stage.title = "GraphAuth - Change Graph Token";
    stage.set_size(width, height);
    stage.connect("destroy", Clutter.main_quit);

    return stage;
}

//Setup window that contains Clutter stage
let stage = setup_stage(620,660);
stage.show();

//New graphAuthPrompt actor
//This actor handles all PAM messages and will start Clutter
let graphAuthPrompt = new GraphAuthPrompt.GraphAuthPrompt();
stage.add_child(graphAuthPrompt.get_actor());

//Begin Authentication for current user
graphAuthPrompt.begin_chauthtok();
