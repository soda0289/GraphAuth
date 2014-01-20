const Lang = imports.lang;
const Signals = imports.signals

const Clutter = imports.gi.Clutter;
const Cairo = imports.cairo;

const Graph = function(size, style){
    this.size = size;
    this.style = style;

    this.Edge = function(src, dest){
        this.src = src;
        this.dest = dest;
        this.width = this.style.edgeWidth;
            

        this.draw = function(canvas, cr, width, height){
            let src_x, src_y, dest_x, dest_y;
            src_x = this.src.position.x;
            src_y = this.src.position.y;
            dest_x = this.dest.position.x;
            dest_y = this.dest.position.y;

            let angle = Math.atan2(dest_x - src_x, dest_y - src_y);
        
            if(src.selected){
                let opp = Math.cos(angle) *  src.radius;
                let adj = Math.sin(angle) *  src.radius;

                src_y += opp;
                src_x += adj;
            }

            if(dest.selected){
                let opp = Math.cos(angle) *  dest.radius;
                let adj = Math.sin(angle) *  dest.radius;

                dest_y -= opp ;
                dest_x -= adj ;
            }
            
            cr.setLineWidth (this.width);
            cr.moveTo(src_x,src_y);
            cr.lineTo(dest_x, dest_y);
            cr.stroke ();
        };

    };

    this.Edge.prototype.style = this.style;


    this.Vertex = function (position) {
        this.position = position;
        this.edge = null;
        this.selected = false;
        this.width = this.style.vertexWidth;
        this.radius = this.style.ringRadius;
        this.ringWidth = this.style.ringWidth;
      
        this.position.x += this.radius;
        this.position.y += this.radius;


        this.distance_to = function(dest){
            return Math.sqrt(Math.pow(this.position.x - dest.position.x, 2) + Math.pow(this.position.y - dest.position.y, 2));
        };

        this.add_edge = function(dest_vertex){
            this.edge = new this.Edge(this, dest_vertex);
            
            return true;
        };

        this.draw = function(canvas, cr){
            if(this.selected){
                cr.setLineWidth (this.ringWidth);
                cr.arc(this.position.x, this.position.y, this.radius, 0, Math.PI * 2);
                cr.stroke();
            }
            
            cr.setLineWidth (this.width);
            cr.arc (this.position.x, this.position.y, 0.0, 0, Math.PI * 2);
            cr.stroke();

            //Draw edge to next vertex
            if(this.edge)
                this.edge.draw(canvas, cr);
        };
    };

    this.Vertex.prototype.style = this.style;
    this.Vertex.prototype.Edge = this.Edge;

    //2D Array of all the vertices
    this.vertices = new Array();
    
    let i, j;
    for(i = 0; i < this.size.x; i++){
        this.vertices[i] = new Array();
        for(j = 0; j < this.size.y; j++){ 
            let pos = new Clutter.Point();
            pos.init(i/this.size.x + this.style.vertexWidth, j/this.size.y + this.style.vertexWidth);
            this.vertices[i][j] = new this.Vertex(pos);
            this.vertices[i][j].index = (i * this.size.y) + j;
        }
    }

     this.ordered_edges = new Array();
    
    //Class Methods
    this.reset = function(){
        let i, j;
        for(i = 0; i < this.size.x; i++){
            for(j = 0; j < this.size.y; j++){ 
                let vertex = this.get_vertex(i, j);
                vertex.selected = false;
                vertex.edge = null;
            }
        }
        
        return true;
    }   

    this.get_vertex = function(x,y){
        return this.vertices[x][y];
    }

    this.draw = function(cairo, cr){
        cr.setLineCap (Cairo.LineCap.ROUND);
        
        let i, j;
        for(i = 0; i < this.vertices.length; i++){
            for(j = 0; j < this.vertices[i].length; j++){
                let vertex = this.vertices[i][j];
                vertex.draw(cairo, cr);
            }
        }

        return true;  
    };
};

const GraphAuth = new Lang.Class({
    Name: "graphAuth",

    _init: function(){

        this.key = null;

        this.width = 660;
        this.height = 660;

        this.first_vertex = null;
        this.last_vertex = null;

        //Setup Canvas Actor for drawing
        this.canvas = new Clutter.Canvas();
        this.canvas.set_size(this.width, this.height);
        this.canvas.connect("draw", Lang.bind(this, this._draw_graph));

        this.canvas_actor = new Clutter.Actor();
        this.canvas_actor.set_size(this.width, this.height);
        this.canvas_actor.set_content(this.canvas);

        this.actor = this.canvas_actor;

        let size = {
            x: 3,
            y: 3
        };

        let style = {
            vertexWidth: 0.05,
            ringRadius: 0.05,
            ringWidth: 0.02,
            edgeWidth: 0.02
        };

        //Setup Graph 
        this._graph = new Graph(size, style);

        //Setup Mouse/Touch 
        let point = new Clutter.Point();
        this.mouse_vertex = new this._graph.Vertex(point);
        this.mouse_vertex.selected = false;
        this.mouse_vertex.width = 0.0;

        this.canvas_actor.connect("motion-event", Lang.bind(this, this._update_mouse_vertex));
        this.canvas_actor.connect("touch-event", Lang.bind(this, this._update_mouse_vertex));
        this.canvas_actor.connect("leave-event", Lang.bind(this, this._update_mouse_vertex));
        this.canvas_actor.set_reactive(true);

        
        //Set Canvcas to invalid to force inital draw
        this.canvas.invalidate();
    },

    get_actor: function(){
        return this.actor;
    },

    _draw_graph: function (canvas, cr, width, height){
        cr.save ();
        cr.setOperator (Cairo.Operator.CLEAR);
        cr.paint ();
        cr.restore ();
        cr.setOperator (Cairo.Operator.OVER);
        cr.scale (width, height);
       
        cr.setLineCap (Cairo.LineCap.ROUND);
        
        this._graph.draw(canvas, cr, width, height);
        if(this.last_vertex)
            this.last_vertex.add_edge(this.mouse_vertex);

        this.mouse_vertex.draw(canvas, cr, width, height);

        return true;
    },

    _generate_key: function (){
        /* Follow vertex edges from first */
        /* vertex to last unless count is */
        /* greater than the number of vertices      */

        this.key = 0;
        if(this.first_vertex != null){
            let edge = this.first_vertex.edge;
            if(edge != null){ 
                let max = this._graph.size.x * this._graph.size.y;
                for(let i = 0; i < max && edge != null;i++){
                    this.key += edge.src.index * Math.pow(max, i);
                    edge = edge.dest.edge;
                }
            }
        }

    },

    _print_pos: function (actor, ev){
        let point = new Clutter.Point();
        ev.get_position(point);
        print("X: " + point.x + " Y: " + point.y);

        return false;
    },

    _new_auth_message: function (ev, msg){
        print("MSG: " + msg);

        return this.key.toString();
    },

    _check_mouse_collision: function(){
        let x = Math.floor(this.mouse_vertex.position.x * this._graph.size.x)
        let y = Math.floor(this.mouse_vertex.position.y * this._graph.size.y)
        let vertex = this._graph.get_vertex(x,y);
        if(vertex == undefined){
            print("Error no vertex found at X: " + " Y: " + y);
            return false;
        }

        //Check Collision
        if(Math.abs(vertex.distance_to(this.mouse_vertex)) < 0.08 && vertex.selected == false){
            vertex.selected = true;

            if(this.last_vertex == null){
                this.first_vertex = 
                this.last_vertex = vertex;

            }else if (this.last_vertex !== vertex){
                this.last_vertex.add_edge(vertex);
                this.last_vertex = vertex;      
            }
        }

        return false;
    },

    _update_mouse_vertex: function (actor, ev){
        let mouse = new Clutter.Point();

        ev.get_position(mouse);

        let aw = this.canvas_actor.get_width();
        let ah = this.canvas_actor.get_height();

        //Do some math and get the mouse relative to the actor instead of the stage
        let aaa = this.canvas_actor.transform_stage_point(mouse.x, mouse.y);

        if(ev.type() == Clutter.EventType.TOUCH_END || ev.type() == Clutter.EventType.LEAVE){
            let err = 0;
            let status = 0;

            this._generate_key();
            this.emit('next');

            this._graph.reset();
            this.last_vertex = null; 
        }else{
            this.mouse_vertex.position.x = aaa[1] / aw;
            this.mouse_vertex.position.y = aaa[2] / ah;

            this._check_mouse_collision();
        }
        //ReDraw
        this.canvas.invalidate();

        return false;   
    },

    show: function(){
        this.actor.show();
    },

    clear: function(){
        if(this.graph != undefined){
            this.graph.reset();
        }
    }

});
Signals.addSignalMethods(GraphAuth.prototype);
