const Lang = imports.lang;
const Signals = imports.signals

const Clutter = imports.gi.Clutter;
const GraphAuth = imports.graphAuth;
const GraphAuthPam = imports.gi.GraphAuth;

const GraphAuthPrompt = new Lang.Class({
	Name: "GraphAuthPromt",

	_init: function() {
		this.actor = new Clutter.Actor();

		//New graphAuth actor used to ask user for code
		this.graph_auth = new GraphAuth.GraphAuth();
		//Quit Clutter as we got message from PAM
		this.graph_auth.connect("next", Clutter.main_quit);
		this.graph_auth.actor.hide();
		this.actor.add_child(this.graph_auth.get_actor());

		this.msg_text = new Clutter.Text();
		this.msg_text.set_text("mesg");
		this.actor.add_child(this.msg_text);

		this.resp_text = new Clutter.Text();
		this.resp_text.set_text("resp");
		this.actor.add_child(this.resp_text);

		//PAM authentication gobject
		this.pam_auth = new GraphAuthPam.Pam();
		//new-message signal is PAM conversation
		//we show the graphAuth actor and wait for user to swipe code.
		this.pam_auth.connect("new-message", Lang.bind(this, this._new_auth_message));
	},

	_new_auth_message: function(signal, message) {
		this.msg_text.set_text(message);
		this.graph_auth.show();

		//Show GraphAuth stage
    	Clutter.main();

    	return this.graph_auth.get_key();
	},

	get_actor: function() {
		return this.actor;
	},

	show: function() {
		this.actor.show();
	},

	hide: function() {
		this.actor.hide();
	},

	begin_auth: function () {
		let err = 0;
		let status = this.pam_auth.authenticate(err);
		if(status[0] == 1){
			this.resp_text.set_text("Authenticated!");
		}else{
			this.resp_text.set_text("Authentication Failed. Error code" + status[0].toString());
	   } 
	},

	begin_chauthtok: function () {
		let err = 0;
	
		let status = this.pam_auth.chauthtok(err);
		if(status[0] == 1){
			this.resp_text.set_text("Changed authentication token!");
		}else{
			this.resp_text.set_text("Changing authentication token failed. Error code" + status.toString());
	   }
	}


});

