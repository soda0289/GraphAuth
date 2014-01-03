const Clutter = imports.gi.Clutter;
imports.searchPath.unshift("./");
const GraphAuth = imports.graphAuth;

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
stage.set_size(520,520);
stage.connect("destroy", Clutter.main_quit);


let ga = new GraphAuth.graphAuth(stage);

ga.show();
stage.show();
Clutter.main();
