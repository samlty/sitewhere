<!-- Dialog for user create/update -->
<div id="uc-dialog" class="modal hide">
	<div class="modal-header k-header">
		<button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
		<h3 id="uc-dialog-header" data-i18n="includes.userCreateDialog.CreateUser"></h3>
	</div>
	<div class="modal-body">
		<div id="uc-tabs">
			<ul>
				<li class="k-state-active" data-i18n="includes.userCreateDialog.UserDetails"></li>
				<li data-i18n="includes.userCreateDialog.Permissions"></li>
				<li data-i18n="public.Metadata"></li>
			</ul>
			<div>
				<form id="uc-user-form" class="form-horizontal" style="padding-top: 20px;">
					<div class="control-group">
						<label class="control-label" for="uc-username" data-i18n="public.Username"></label>
						<div class="controls">
							<input type="text" id="uc-username" class="input-xlarge" title="Username">
						</div>
					</div>
					<div class="control-group">
						<label class="control-label" for="uc-password" data-i18n="public.Password"></label>
						<div class="controls">
							<input type="password" id="uc-password" class="input-xlarge" title="Password">
						</div>
					</div>
					<div class="control-group">
						<label class="control-label" for="uc-password-confirm"
							data-i18n="includes.userCreateDialog.PasswordConfirm"></label>
						<div class="controls">
							<input type="password" id="uc-password-confirm" class="input-xlarge" title="Password Confirm">
						</div>
					</div>
					<div class="control-group">
						<label class="control-label" for="uc-first-name" data-i18n="public.FirstName"></label>
						<div class="controls">
							<input type="text" id="uc-first-name" class="input-xlarge" title="First Name">
						</div>
					</div>
					<div class="control-group">
						<label class="control-label" for="uc-last-name" data-i18n="public.LastName"></label>
						<div class="controls">
							<input type="text" id="uc-last-name" class="input-xlarge" title="Last Name">
						</div>
					</div>
					<div class="control-group">
						<label class="control-label" for="uc-account-status"
							data-i18n="includes.userCreateDialog.AccountStatus"></label>
						<div class="controls">
							<input id="uc-account-status" />
						</div>
					</div>
				</form>
			</div>
			<div>
				<div id="uc-permissions" style="height: 250px; border: 0px; margin: 10px;"></div>
			</div>
			<div>
				<div id="uc-metadata">
					<jsp:include page="../includes/metadata.jsp" flush="true">
						<jsp:param name="uid" value="uc" />
					</jsp:include>
				</div>
			</div>
		</div>
	</div>
	<input id="uc-original-username" type="hidden" value="" />
	<div class="modal-footer">
		<a href="javascript:void(0)" class="btn" data-dismiss="modal" data-i18n="public.Cancel"></a> <a
			id="uc-dialog-submit" href="javascript:void(0)" class="btn btn-primary" data-i18n="public.Create"></a>
	</div>
</div>

<!-- Script support for site create/update dialog -->
<script>
	/** Function called when dialog is submitted */
	var ucSubmitCallback;

	/** Pointer to tabs instance */
	var ucTabs;

	/** List of available authorities */
	var authsDS;

	/** Tree widget for authorities */
	var authsTree;

	/** Dropdown list of account statuses */
	var ucStatusList;

	$(document).ready(
		function() {
			var ucAccountStatuses = [ {
				text : i18next("includes.Active"),
				status : "Active"
			}, {
				text : i18next("includes.Expired"),
				status : "Expired"
			}, {
				text : i18next("includes.Locked"),
				status : "Locked"
			}, ];

			/** Create the tab strip */
			ucTabs = $("#uc-tabs").kendoTabStrip({
				animation : false
			}).data("kendoTabStrip");

			// create DropDownList from input HTML element
			ucStatusList = $("#uc-account-status").kendoDropDownList({
				dataTextField : "text",
				dataValueField : "status",
				dataSource : ucAccountStatuses,
			}).data("kendoDropDownList");

			/** Create AJAX datasource for authorities list */
			authsDS = new kendo.data.HierarchicalDataSource({
				transport : {
					read : {
						url : "${pageContext.request.contextPath}/api/authorities/hierarchy",
						beforeSend : function(req) {
							req.setRequestHeader('Authorization', "Basic ${basicAuth}");
							req.setRequestHeader('X-SiteWhere-Tenant', "${tenant.authenticationToken}");
						},
						dataType : "json",
					}
				},
				schema : {
					model : {
						id : "id",
						hasChildren : "group",
						children : "items",
					}
				}
			});

			/** Create the authorities tree */
			authsTree = $("#uc-permissions").kendoTreeView({
				loadOnDemand : false,
				dataSource : authsDS,
				checkboxes : {
					checkChildren : true
				},
			}).data("kendoTreeView");

			/** Handle dialog submit */
			$('#uc-dialog-submit').click(
				function(event) {
					event.preventDefault();
					var username = $('#uc-original-username').val();
					var password = $('#uc-password').val();
					var isCreate = (username.length == 0);
					if ((isCreate) && (!ucValidateForCreate())) {
						return;
					}
					if ((!isCreate) && (!ucValidateForUpdate())) {
						return;
					}
					var auths = ucGetCheckedAuths();
					var userdata = {
						"username" : $('#uc-username').val(),
						"firstName" : $('#uc-first-name').val(),
						"lastName" : $('#uc-last-name').val(),
						"authorities" : auths,
						"status" : ucStatusList.dataItem().status,
						"metadata" : swMetadataAsLookup(ucMetadataDS.data()),
					};
					if (password.length > 0) {
						userdata.password = password;
					}
					if (isCreate) {
						$.postAuthJSON("${pageContext.request.contextPath}/api/users", userdata,
							"${basicAuth}", "${tenant.authenticationToken}", onSuccess, onCreateFail);
					} else {
						$.putAuthJSON("${pageContext.request.contextPath}/api/users/" + username, userdata,
							"${basicAuth}", "${tenant.authenticationToken}", onSuccess, onUpdateFail);
					}
				});

			/** Called on successful create/update */
			function onSuccess() {
				$('#uc-dialog').modal('hide');
				if (ucSubmitCallback != null) {
					ucSubmitCallback();
				}
			}

			/** Handle failed call to create user */
			function onCreateFail(jqXHR, textStatus, errorThrown) {
				handleError(jqXHR, "Unable to create user.");
			}

			/** Handle failed call to update user */
			function onUpdateFail(jqXHR, textStatus, errorThrown) {
				handleError(jqXHR, "Unable to update user.");
			}
		});

	function uncheckNodes(nodes) {
		for (var i = 0; i < nodes.length; i++) {
			nodes[i].set("checked", false);
			if (nodes[i].hasChildren) {
				uncheckNodes(nodes[i].children.data());
			}
		}
	}

	function checkedNodeIds(nodes, checkedNodes) {
		for (var i = 0; i < nodes.length; i++) {
			if (nodes[i].checked) {
				checkedNodes.push(nodes[i].id);
			}
			if (nodes[i].hasChildren) {
				checkedNodeIds(nodes[i].children.view(), checkedNodes);
			}
		}
	}

	/** Get a list of checked auths */
	function ucGetCheckedAuths() {
		var checkedNodes = [];
		checkedNodeIds(authsDS.view(), checkedNodes);
		return checkedNodes;
	}

	/** Reset authorities to unchecked */
	function ucResetCheckedAuths() {
		uncheckNodes(authsDS.data());
	}

	/** Check the auths found in the given list */
	function ucSetCheckedAuths(userAuths) {
		for (var i = 0; i < userAuths.length; i++) {
			var data = authsDS.get(userAuths[i]);
			if (data) {
				data.set("checked", true);
			}
		}
	}

	/** Validate for create mode */
	function ucValidateForCreate() {
		$.validity.setup({
			outputMode : "label"
		});
		$.validity.start();

		/** Validate user form */
		$("#uc-username").require();
		$("#uc-password").require();
		$("#uc-password-confirm").require();
		$("#uc-first-name").require();
		$("#uc-last-name").require();
		$("#uc-password").assert(function(element) {
			var pwd = $("#uc-password").val();
			var pwdConfirm = $("#uc-password-confirm").val();
			if ((pwd.length == 0) || (pwd == pwdConfirm)) {
				return true;
			}
			return false;
		}, "Passwords must match.");

		var result = $.validity.end();
		return result.valid;
	}

	/** Validate for update mode */
	function ucValidateForUpdate() {
		$.validity.setup({
			outputMode : "label"
		});
		$.validity.start();

		/** Validate user form */
		$("#uc-username").require();
		$("#uc-first-name").require();
		$("#uc-last-name").require();
		$("#uc-password").assert(function(element) {
			var pwd = $("#uc-password").val();
			var pwdConfirm = $("#uc-password-confirm").val();
			if ((pwd.length == 0) && (pwdConfirm.length == 0)) {
				return true;
			}
			if (pwd == pwdConfirm) {
				return true;
			}
			return false;
		}, "Passwords must match.");

		var result = $.validity.end();
		return result.valid;
	}

	/** Open the user create dialog */
	function ucOpen(callback) {
		ucClearDialog();
		$('#uc-dialog-header').html(i18next("includes.userCreateDialog.CreateUser"));
		$('#uc-dialog-submit').html(i18next("public.Create"));
		ucMetadataDS.data(new Array());
		ucTabs.select(0);

		// Indicates create vs update.
		$('#uc-original-username').val("");

		// Make sure authorities tree is expanded.
		ucResetCheckedAuths();
		authsTree.expand(".k-item");

		// Function called on submit.
		ucSubmitCallback = callback;

		$.validity.clear();
		$('#uc-dialog').modal('show');
	}

	/** Called to load the given user and open it for edit */
	function uuOpen(username, callback) {
		// Function called on submit.
		ucSubmitCallback = callback;

		// Get site for edit.
		$.getAuthJSON("${pageContext.request.contextPath}/api/users/" + username, "${basicAuth}",
			"${tenant.authenticationToken}", uuUserGetSuccess, uuUserGetFailed);
	}

	/** Called on successful user load request */
	function uuUserGetSuccess(data, status, jqXHR) {
		$('#uc-dialog-header').html(i18next("includes.EditUser"));
		$('#uc-dialog-submit').html(i18next("public.Save"));

		// Load text fields.
		$('#uc-original-username').val(data.username);
		$('#uc-username').val(data.username);
		$('#uc-password').val("");
		$('#uc-password-confirm').val("");
		$('#uc-first-name').val(data.firstName);
		$('#uc-last-name').val(data.lastName);

		// Check current granted authorities.
		ucResetCheckedAuths();
		authsTree.expand(".k-item");
		ucSetCheckedAuths(data.authorities);

		// Select correct status.
		ucStatusList.value(data.status);

		ucMetadataDS.data(swLookupAsMetadata(data.metadata));
		ucTabs.select(0);

		$.validity.clear();
		$('#uc-dialog').modal('show');
	}

	/** Handle error on getting user */
	function uuUserGetFailed(jqXHR, textStatus, errorThrown) {
		handleError(jqXHR, "Unable to load user for edit.");
	}

	/** Clear all dialog fields */
	function ucClearDialog() {
		$('#uc-user-form')[0].reset();
	}
</script>