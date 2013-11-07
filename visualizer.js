var mode_count = 0;

var link_distance = 70;
var charge = -1000;

var current_index;
var info_clicked = false;

function colors(i) {
    switch (i) {
        case 0:
            return "#E15E6E";
        case 1:
            return "#E1918E";
        case 2:
            return "#E0C4AE";
        default:
            return "#B4DEBB"
    }
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
        .attr("fill", function (d, i) {
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
    d3.selectAll(".node").each(function (d, i){
        if(i == current_index) {
            d3.select(this.firstElementChild).style('stroke', 'grey');
            return null;
        }
    });
}

function set_info(index) {
    clear_info();

    if($("#url-container").text().trim() == "CLICK A NODE") {
        $("#list-container").append("\<ul><li id=\"wapiti-list\" class=\"wapiti-text-color\">WAPITI<ul> </ul> </li> <hr> <li id=\"skipfish-list\" class=\"skipfish-text-color\">SKIPFISH <ul> </ul> </li> <hr> <li id=\"arachni-list\" class=\"arachni-text-color\">ARACHNI <ul> </ul> </li> </ul>");
        info_clicked = true;
    }

    header = params.urls[index].url;

    if (header.length > 55) {
        header = header.substr(0, 55) + "...";
    }

    $("#url-container").text(header);
    var wapiti;
    var skipfish;
    var arachni;

    var wapiti_count;
    var skipfish_count;
    var arachni_count;

    switch (mode_count) {
        case 0:
            wapiti = params.urls[index].wapiti.sqli;
            skipfish = params.urls[index].skipfish.sqli;
            arachni = params.urls[index].arachni.sqli;
            wapiti_count = dataset.nodes[index].sqli.wapiti;
            skipfish_count = dataset.nodes[index].sqli.skipfish;
            arachni_count = dataset.nodes[index].sqli.arachni;
            break;
        case 1:
            wapiti = params.urls[index].wapiti.xss;
            skipfish = params.urls[index].skipfish.xss;
            arachni = params.urls[index].arachni.xss;
            wapiti_count = dataset.nodes[index].xss.wapiti;
            skipfish_count = dataset.nodes[index].xss.skipfish;
            arachni_count = dataset.nodes[index].xss.arachni;
            break;
        case 2:
            wapiti = params.urls[index].wapiti.sqli.concat(params.urls[index].wapiti.xss);
            skipfish = params.urls[index].skipfish.sqli.concat(params.urls[index].skipfish.xss);
            arachni = params.urls[index].arachni.sqli.concat(params.urls[index].arachni.xss);
            wapiti_count = dataset.nodes[index].both.wapiti;
            skipfish_count = dataset.nodes[index].both.skipfish;
            arachni_count = dataset.nodes[index].both.arachni;
            break;
        default:
            break;
    };

    $("#list-container").customScrollbar();

    $('#wapiti-list ul').append("<li class='inner-list'>ERRORS: " + wapiti_count + "</li>");
    $('#skipfish-list ul').append("<li class='inner-list'>ERRORS: " + skipfish_count + "</li>");
    $('#arachni-list ul').append("<li class='inner-list'>ERRORS: " + arachni_count + "</li>");


    for (var k = 0; k < wapiti.length; k++) {
        $('#wapiti-list ul').append("<li class='inner-list'>\"" + wapiti[k].injection_value.trim() + "\"</li>");
    }
    for (var k = 0; k < skipfish.length; k++) {
        $('#skipfish-list ul').append("<li class='inner-list'>\"" + skipfish[k].injection_value.trim() + "\"</li>");
    }
    for (var k = 0; k < arachni.length; k++) {
        $('#arachni-list ul').append("<li class='inner-list'>\"" + arachni[k].injection_value.trim() + "\"</li>");
    }

    $("#list-container").customScrollbar({
        skin: "default-skin",
        hScroll: false,
        updateOnWindowResize: true
    })

    current_index = index;
}

function clear_info() {
    $('#wapiti-list ul').empty();
    $('#skipfish-list ul').empty();
    $('#arachni-list ul').empty();
    $("#list-container").customScrollbar("remove");
}

function loader() {
    var w = 1000;
    var h = 600;

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
		div.html(d.node)
			.style("left", (d3.event.pageX) + "px")
			.style("top", (d3.event.pageY - 28) + "px");
	})
	.on("mouseout", function(d) {
		div.transition()
			.duration(500)
			.style("opacity", 0);
	});

    nodes.on("click", function (d, i) { 
        clear_selected();
        set_info(i);
        add_selected();
    });

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

    d3.select('#overlay-rect').on("click", function (d, i) {
        switch (mode_count) {
            case 0:
                d3.select('#overlay-title').text("XSS");
                break;
            case 1:
                d3.select('#overlay-title').text("BOTH");
                break;
            case 2:
                d3.select('#overlay-title').text("SQLI");
                break;
            default:
                break;
        }
        mode_count = (mode_count + 1) % 3;
        create_pie(nodes);
        if(info_clicked) {
            set_info(current_index);
            add_selected();
        }
    });
}