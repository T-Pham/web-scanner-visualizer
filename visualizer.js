var mode_count = 0;

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

function neighbor(i) {
    var edges = dataset['edges'];
    var result = [];
    for (k = 0; k < edges.length; k++) {
        if (edges[k]['source'].index === i) {
            result.push(edges[k]['target'].index);
        }
        else {
            if (edges[k]['target'].index === i) {
                result.push(edges[k]['source'].index);
            }
        }
    }
    return result;
}

function create_pie(nodes, error_type) {
    var pie = d3.layout.pie();
    pie.sort(null);

    nodes.each(function (d, i) {
        var arc;
        var total;
        var average;
        var dataset;

        switch (error_type) {
            case "sqli":
                total = d.sqli.total;
                average = sqli_average;
                dataset = d.sqli.pie;
                break;
            case "xss":
                total = d.xss.total;
                average = xss_average;
                dataset = d.xss.pie;
                break;
            case "both":
                total = d.both.total;
                average = both_average;
                dataset = d.both.pie;
                break;
            default:
                return false;
        }

        if (total == 0) {
            arc = d3.svg.arc()
                    .innerRadius(0)
                    .outerRadius(8);
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

            arc = d3.svg.arc()
                    .innerRadius(0)
                    .outerRadius(outerRadius);
        }

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

function loader() {
    var w = 1000;
    var h = 600;
    var link_distance = 55;
    var charge = -900;

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
            .call(force.drag);

    create_pie(nodes, "sqli");

    nodes.append("svg:title")
            .text(function (d) { return d.node.valueOf(); });


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
                create_pie(nodes, "xss");
                d3.select('#overlay-title').text("XSS");
                break;
            case 1:
                create_pie(nodes, "both");
                d3.select('#overlay-title').text("BOTH");
                break;
            case 2:
                create_pie(nodes, "sqli");
                d3.select('#overlay-title').text("SQLI");
                break;
            default:
                break;
        }
        mode_count = (mode_count + 1) % 3;
    });
}