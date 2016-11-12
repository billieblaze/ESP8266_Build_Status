//var statusURL = 'http://192.168.1.106/status';
//var versionURL = 'http://192.168.1.106/version';

var statusURL = 'http://10.0.0.23/status';
var versionURL = 'http://10.0.0.23/version';

angular.module("ufo", [])
  .controller('statusController', function($scope, $http, $interval) {
  $scope.error = "";
  $scope.status = {};

   var refreshData = function(){
   	 $http({
	   method: 'GET',
	   url: statusURL
	  }).then(function successCallback(response) {
  	  	$scope.status = response.data;
  	  }, function errorCallback(response) {
  		$scope.error = response;
  	  });
  	}

  	refreshData();

  	$interval(function(){
  		console.log('polling server');
  		refreshData();
  	}, 15000);

  })

  .controller('versionController', function($scope, $http, $interval) {

		$scope.status = {};
		$scope.version = { latest: "", staging:"", production:""};

		var getVersion = function(server){
			$http({
				method: 'GET',
				url: versionURL + '/' + server,
				responseType:'text'
			}).then(function successCallback(response) {
			  	$scope.version[server] = response.data[server];
		    });
		}

		getVersion('latest');
		getVersion('staging');
		getVersion('production');

		$interval(function(){
			console.log('polling environments for version details');
			getVersion('staging');

		}, 60000);

		$interval(function(){
			console.log('polling staging for version details');

			getVersion('staging');


		}, 80000);

		$interval(function(){
			console.log('polling production for version details');
			getVersion('production');

		}, 120000);


  })

  .controller('logController', function($scope) {
  })

  .controller('testController', function($scope, $http) {
    $scope.test = {"environment":"", "status":""};

    $scope.sendTest = function(){
    	$http({
		  method: 'POST',
		  url: statusURL,
		  data: $scope.test
		}).then(function successCallback(response) {
	  	  	$scope.status = response.data;
	  	}, function errorCallback(response) {
	  		$scope.error = response;
	  	});
    }
  })

  .directive("status", function($timeout){
    return {
      template: "<div><i class='glyphicon glyphicon-{{icon}}'></i><h4>{{title}}</h4><span class='text-muted'>{{build}}</span></div>",
      replace: true,
      scope: { icon: '@', title: '@', build: '@'},
      link: function( scope, element, attrs){
      	scope.icon="refresh";

      	attrs.$observe('build', function(value){
	  		switch (attrs.build) {
	    		case "CONNECTING":
			       scope.icon = "refresh";
			       break;
			    case "BUILDING":
			       scope.icon = "wrench";
			       break;
			    case "DEPLOYING":
			       scope.icon = "	";
			       break;
				case "TESTING":
			       scope.icon = "tasks";
			       break;
				case "STABLE":
			       scope.icon = "check";
			       break;
				case "UNSTABLE":
			       scope.icon = "warning-sign";
			       break;
				case "FAILED":
			       scope.icon = "fire";
			       break;
				case "ALARM":
			       scope.icon = "bullhorn";
			       break;
			}
		});
	  }
	}

  })