var filters = {
    /* Filter functions return true if the given plugin passes the filter and
       should be displayed, otherwise false. */

    hiddenPluginNo: 0,
    hiddenMessageNo: 0,

    searchFilter: function(plugin, needle) {
        if (needle.length == 0) {
            return true;
        }

        if (plugin.name.toLowerCase().indexOf(needle) != -1
            || plugin.getCrcString().toLowerCase().indexOf(needle) != -1
            || plugin.version.toLowerCase().indexOf(needle) != -1) {

            return true;
        }

        var tags = plugin.getTagStrings();
        if (tags.added.toLowerCase().indexOf(needle) != -1
            || tags.removed.toLowerCase().indexOf(needle) != -1) {

            return true;
        }

        for (var i = 0; i < plugin.messages.length; ++i) {
            if (plugin.messages[i].content[0].str.toLowerCase().indexOf(needle) != -1) {
                return true;
            }
        }
    },

    messagelessFilter: function(plugin) {
        /* This function could be further optimised to perform fewer checks,
           but it's also responsible for setting the hidden message count, so
           has to go through everything. */

        var hasMessages = false;
        /* If any messages exist, check if they are hidden or not. Note
           that the messages may not be present as elements, so the check
           is actually if they would be hidden according to the message
           filters. */
        if (this.allMessageFilter()) {
            plugin.messages.forEach(function(message){
                if (this.noteFilter(message)
                    && this.doNotCleanFilter(message)) {

                    hasMessages = true;
                    return;
                }
                ++hiddenMessageNo;
            }, this);
        } else {
            hiddenMessageNo += plugin.messages.length;
        }

        if (document.getElementById('hideMessagelessPlugins').checked) {
            return hasMessages;
        } else {
            return true;
        }
    },

    applyPluginFilters: function(plugins) {
        var search = document.getElementById('searchBox').value.toLowerCase();
        hiddenPluginNo = 0;
        hiddenMessageNo = 0;
        var filteredPlugins = [];

        plugins.forEach(function(plugin){
            /* Messageless filter needs to run first. */
            if (this.messagelessFilter(plugin)
                && this.searchFilter(plugin, search)) {

                filteredPlugins.push(plugin);
                return;
            }
            ++hiddenPluginNo;
        }, this);

        document.getElementById('hiddenPluginNo').textContent = hiddenPluginNo;
        document.getElementById('hiddenMessageNo').textContent = hiddenMessageNo;

        return filteredPlugins;
    },

    /* Message filter functions are run from within plugin cards, when the card's
       messages are to be added as elements. Each filter should return true if the
       message is to be displayed. */

    noteFilter: function(message) {
        if (document.getElementById('hideNotes').checked) {
            return message.type != 'say';
        } else {
            return true;
        }
    },

    doNotCleanFilter: function(message) {
        if (document.getElementById('hideDoNotCleanMessages').checked) {
            return message.content[0].str.indexOf(l10n.jed.translate("Do not clean").fetch()) == -1;
        } else {
            return true;
        }
    },

    allMessageFilter: function() {
        return !document.getElementById('hideAllPluginMessages').checked;
    },

    applyMessageFilters: function(messages) {
        var filteredMessages = [];

        if (this.allMessageFilter()) {
            messages.forEach(function(message){
                if (this.noteFilter(message)
                    && this.doNotCleanFilter(message)) {

                    filteredMessages.push(message);
                    return;
                }
            }, this);
        }

        return filteredMessages;
    },
};

function setFilteredUIData(evt) {
    var filtered = filters.applyPluginFilters(loot.game.plugins);
    document.getElementById('cardsNav').lastElementChild.data = filtered;
    document.getElementById('main').lastElementChild.data = filtered;

    /* Also run message filters on the current card elements. */
    var cards = document.getElementById('main').getElementsByTagName('loot-plugin-card');
    for (var i = 0; i < cards.length; ++i) {
        cards[i].onMessagesChange();  // Calls Plugin.getUIMessages(), which calls filters.applyMessageFilters().
    }
}