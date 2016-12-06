function cputemp2(){
    $.ajax( {
        type: "POST",
        url: "../cputemp.py",
        dataType: "html",
        success: function(msg)
        {
        document.getElementById('swiss').innerHTML = msg;
        },
	});
    }
