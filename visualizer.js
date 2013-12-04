var mode_count = 0;

var link_distance = 20;
var charge = -900;

var current_index;
var info_clicked = false;

function colors(i) {
    switch (i) {
        case 0:
            return "arachni-color";
        case 1:
            return "w3af-color";
        case 2:
            return "wapiti-color";
        default:
            return "error-free-color"
    }
}

function level_of_node_with_url(url) {
	for (var i = 0; i < dataset.edges.length; i++) {
		var edge = dataset.edges[i];
		if (url == edge.target.node) return 1 + level_of_node_with_url(edge.source.node);
	}
	return 0;
}

function create_pie(nodes) {
    var pie = d3.layout.pie();
    pie.sort(null);

    nodes.each(function (d, i) {
        var arc;
        var total;
        var average;
        var dataset;

        switch (mode_count) {
            case 0:
                total = d.sqli.total;
                average = sqli_average;
                dataset = d.sqli.pie;
                break;
            case 1:
                total = d.xss.total;
                average = xss_average;
                dataset = d.xss.pie;
                break;
            case 2:
                total = d.both.total;
                average = both_average;
                dataset = d.both.pie;
                break;
            default:
                return false;
        }

        if (total == 0) {
            outerRadius = 8;
        }
        else {
            var outerRadius = total * 15 / average;

            if (outerRadius > 30) {
                outerRadius = 30;
            }
            else {
                if (outerRadius < 10) {
                    outerRadius = 10;
                }
            }
        }

        arc = d3.svg.arc()
            .innerRadius(0)
            .outerRadius(outerRadius);

        d3.select(this)
        .selectAll("circle")
        .remove();

        d3.select(this)
        .append("circle")
        .attr('r', outerRadius + 1)
        .style('stroke', 'none')
        .style('stroke-width', 3)
        .style('fill-opacity', 0);

        d3.select(this)
        .selectAll("g.arc")
        .remove();

        d3.select(this)
        .selectAll("g.arc")
        .data(pie(dataset))
        .enter()
        .append("g")
        .attr("class", "arc")
        .append("path")
        .attr("class", function (d, i) {
            return colors(i);
        })
        .attr("d", arc);
    });
}

function clear_selected(node) {
    d3.selectAll(".node").each(function (d, i){
        if(i == current_index) {
            d3.select(this.firstElementChild).style('stroke', 'none');
            return null;
        }
    });
}

function add_selected(node) {
    $('#urls-select').val(current_index);
    d3.selectAll(".node").each(function (d, i){
        if(i == current_index) {
            d3.select(this.firstElementChild).style('stroke', 'crimson');
            return null;
        }
    });
}

function link_for_tool_error(tool_error) {
	var path = params.urls[current_index].url;
	var method = tool_error.method;
	var query = tool_error.injection_value.trim();
	return Mustache.render("<a href='#' onclick='go_to_path_with_method(\"{{path}}\", \"{{query}}\", \"{{method}}\")'>[" + method + "] \"{{query}}\"</a>", {path: path, method: method, query: query});
}

function go_to_path_with_method(path, query, method) {
	method = method || "get";
	var form = document.createElement("form");
	form.setAttribute("method", method);
	form.setAttribute("action", path);
	form.setAttribute("target", "_blank");
	var parameters = parse_query(query);
	for (var key in parameters) {
		if (parameters.hasOwnProperty(key)) {
			var hiddenField = document.createElement("input");
			hiddenField.setAttribute("type", "hidden");
			hiddenField.setAttribute("name", key);
			hiddenField.setAttribute("value", parameters[key]);
			form.appendChild(hiddenField);
		}
	}
	document.body.appendChild(form);
	HTMLFormElement.prototype.submit.call(form);
}

function parse_query(query) {
	var parameters = {};
	var vars = query.split('&');
	for (var i = 0; i < vars.length; i++) {
		var pair = vars[i].split('=');
        var key = pair[0];
        var value = pair[1];
        var err;

        try {
            key = decodeURIComponent(pair[0]);
        }
        catch(err) {
            console.log("EXCEPTION IN DECODING KEY");            
        }

        try {
            value = decodeURIComponent(pair[1]);
        }
        catch(err) {
            console.log("EXCEPTION IN DECODING VALUE");            
        }

		parameters[key] = value;
    }
	return parameters;
}

function set_info() {
    clear_info();

    if($("#url-container").text().trim() == "CLICK A NODE") {
        $("#list-container").append("\<ul><li id=\"arachni-list\" class=\"arachni-text-color\">ARACHNI<ul> </ul> </li> <hr> <li id=\"w3af-list\" class=\"w3af-text-color\">W3AF<ul> </ul> </li> <hr> <li id=\"wapiti-list\" class=\"wapiti-text-color\">WAPITI<ul> </ul> </li> </ul>");
        info_clicked = true;
    }

    header = params.urls[current_index].url;

    if (header.length > 55) {
        header = header.substr(0, 55) + "...";
    }

    $("#url-container").text(header);
    var wapiti;
    var w3af;
    var arachni;

    var wapiti_count;
    var w3af_count;
    var arachni_count;

    switch (mode_count) {
        case 0:
            wapiti = params.urls[current_index].wapiti.sqli;
            w3af = params.urls[current_index].w3af.sqli;
            arachni = params.urls[current_index].arachni.sqli;
            wapiti_count = dataset.nodes[current_index].sqli.wapiti;
            w3af_count = dataset.nodes[current_index].sqli.w3af;
            arachni_count = dataset.nodes[current_index].sqli.arachni;
            break;
        case 1:
            wapiti = params.urls[current_index].wapiti.xss;
            w3af = params.urls[current_index].w3af.xss;
            arachni = params.urls[current_index].arachni.xss;
            wapiti_count = dataset.nodes[current_index].xss.wapiti;
            w3af_count = dataset.nodes[current_index].xss.w3af;
            arachni_count = dataset.nodes[current_index].xss.arachni;
            break;
        case 2:
            wapiti = params.urls[current_index].wapiti.sqli.concat(params.urls[current_index].wapiti.xss);
            w3af = params.urls[current_index].w3af.sqli.concat(params.urls[current_index].w3af.xss);
            arachni = params.urls[current_index].arachni.sqli.concat(params.urls[current_index].arachni.xss);
            wapiti_count = dataset.nodes[current_index].both.wapiti;
            w3af_count = dataset.nodes[current_index].both.w3af;
            arachni_count = dataset.nodes[current_index].both.arachni;
            break;
        default:
            break;
    };

    $("#list-container").customScrollbar();

    $('#wapiti-list ul').append("<div class='error-field'>" + wapiti_count + " ERROR(S)</div>");
    $('#w3af-list ul').append("<div class='error-field'>" + w3af_count + " ERROR(S)</div>");
    $('#arachni-list ul').append("<div class='error-field'>" + arachni_count + " ERROR(S)</div>");


    for (var k = 0; k < wapiti.length; k++) {
        $('#wapiti-list ul').append("<li class='inner-list'>" + link_for_tool_error(wapiti[k]) + "</li>");
    }
    for (var k = 0; k < w3af.length; k++) {
        $('#w3af-list ul').append("<li class='inner-list'>" + link_for_tool_error(w3af[k]) + "</li>");
    }
    for (var k = 0; k < arachni.length; k++) {
        $('#arachni-list ul').append("<li class='inner-list'>" + link_for_tool_error(arachni[k]) + "</li>");
    }

    $("#list-container").customScrollbar({
        skin: "default-skin",
        hScroll: false,
        updateOnWindowResize: true
	})
}

function clear_info() {
    $('#wapiti-list ul').empty();
    $('#w3af-list ul').empty();
    $('#arachni-list ul').empty();
    $("#list-container").customScrollbar("remove");
}

function populate_url_selects() {
	$('#urls-select').html('');
	var url_filter_term = $('#url_filter_field').val();
	for(var i = 0; i < params.urls.length; i++) {
		if ((url_filter_term.length > 0) && (params.urls[i].url.indexOf(url_filter_term) == -1)) continue;
		var url = params.urls[i].url.length < 80 ? params.urls[i].url : params.urls[i].url.substr(0, 75) + "..."; 
		$('#urls-select').append("<option value=" + i + ">" + url + "</option>");
	}
	$('#urls-select option').sort( function(a, b) {
        return $(a).text() > $(b).text() ? 1 : -1;
    }).appendTo('#urls-select');
}

function apply_url_filtering_effect() {
	d3.selectAll(".node").each(function (d, i) {
		d3.select(this).transition().style('fill-opacity', '1');
		return null;
	});
	var url_filter_term = $('#url_filter_field').val();
	if (url_filter_term.length == 0) return;	
	d3.selectAll(".node").each(function (d, i) {
		if(d.node.indexOf(url_filter_term) == -1) {
			d3.select(this).transition().style('fill-opacity', '0.2');
			return null;
		}
	});
}

function tooltip_html_for_node(node) {
	return Mustache.render("URL: <b>{{url}}</b><br>Level: <b>{{level}}</b>", {url: node.node, level: level_of_node_with_url(node.node)});
}

function highlight_path_to_node_with_url(url) {
    d3.selectAll("line").each(function (d, i) {
		if (d.target.node == url) {
			d3.select(this).style('stroke', 'crimson');
			highlight_path_to_node_with_url(d.source.node);
		}
    });
}

function unhighlight_paths() {
    d3.selectAll("line").each(function (d, i) {
		d3.select(this).style('stroke', '#ccc');
		return null;
    });
}

function loader() {
    var w = $(window).width() - 300;
    var h = $(window).height() - 100;

    d3.select("#visualization")
      .attr("width", w)
      .attr("height", h);

    d3.select("svg")
      .attr("width", w)
      .attr("height", h);

    var graph = d3.select("svg").select("#graph");

    var force = d3.layout.force()
            .nodes(dataset.nodes)
            .links(dataset.edges)
            .size([w, h])
            .linkDistance([link_distance])
            .charge(charge);

    var edges = graph.selectAll("line")
            .data(dataset.edges)
            .enter()
            .append("line")
            .style("stroke", "#ccc")
            .style("stroke-width", 1);

    var nodes = graph.selectAll("g")
            .data(dataset.nodes)
            .enter()
            .append("svg:g")
            .attr("class", "node")
            .call(force.drag);

	var div = d3.select("body")
			.append("div")
			.attr("class", "tooltip")
			.style("opacity", 0);

    create_pie(nodes, "sqli");

    nodes.on("mouseover", function(d) {
		div.transition()
			.duration(200)
			.style("opacity", .9);
		div.html(tooltip_html_for_node(d))
			.style("left", (d3.event.pageX) + "px")
			.style("top", (d3.event.pageY - 28) + "px");
		highlight_path_to_node_with_url(d.node);
	})
	.on("mouseout", function(d) {
		div.transition()
			.duration(500)
			.style("opacity", 0);
		unhighlight_paths();
	});

    nodes.on("click", function (d, i) { 
        clear_selected();
        current_index = i;
        set_info();
        add_selected();
    });


    d3.selectAll(".legend-rect")
    .attr("x", 15)
    .attr("y", function(d, i) {
        return (h - 25*(i+1));
    });

    d3.selectAll(".legend-text")
    .attr("x", 75)
    .attr("y", function(d, i) {
        return (h + 15 - 25*(i+1));
    });

    d3.select("#list-container").style("height", (h-68) + "px");

    force.on("tick", function () {
        edges.attr("x1", function (d) {
            return d.source.x;
        })
        .attr("y1", function (d) {
            return d.source.y;
        })
        .attr("x2", function (d) {
            return d.target.x;
        })
        .attr("y2", function (d) {
            return d.target.y;
        });

        nodes.attr("transform", function (d) { return "translate(" + d.x + "," + d.y + ")"; });

    });

    force.start();

    $('#mode-select').on('change', function() {
        
        mode = $('#mode-select').val();
        switch (mode) {
            case "SQLI":
                mode_count = 0;
                d3.select('#overlay-title').text("SQLI");
                break;
            case "XSS":
                mode_count = 1;
                d3.select('#overlay-title').text("XSS");
                break;
            case "BOTH":
                mode_count = 2;
                d3.select('#overlay-title').text("BOTH");
                break;
            default:
                break;
        }

        create_pie(nodes);
        if (info_clicked) {
            set_info(current_index);
            add_selected();
        }
    });

    populate_url_selects();

    $('#urls-select').on('change', function() {
        clear_selected();
        current_index = $('#urls-select').val();
        set_info();
        add_selected();
    });
	
	$('#url_filter_field').on('keyup', function() {
		populate_url_selects();
		apply_url_filtering_effect();
    });
}
