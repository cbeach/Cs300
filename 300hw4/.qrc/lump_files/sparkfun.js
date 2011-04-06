String.prototype.strstr = function(needle) {
  var unf = this.indexOf(needle) + 1;

  return((unf === 0) ? 0 : 1);
};

String.prototype.stristr = function(needle) {
  var unf = this.toLowerCase().indexOf(needle.toLowerCase()) + 1;

  return((unf === 0) ? 0 : 1);
}

var SparkFun = Class.create({
  initialize: function() {
    this.HTTP_SERVER = window.HTTP_SERVER;
    this.HTTPS_SERVER = window.HTTPS_SERVER;
    this.HTTP_STATIC = window.HTTP_STATIC;
    this.AUTHENTICATED = window.AUTHENTICATED;
    this.currencies = window.CURRENCIES;
    this.cookie_jar = new CookieJar({expires: 7776000, path: '/'}); // 90 days
    this.current_category = this.get_current_category();
    this.full_layout = window.FULL_LAYOUT;
    this.banner = new SparkFunBanner();
    this.freeday_banner = new FreeDayBanner();

    if (this.full_layout) {
      this.render_menu();
      this.render_category();
      this.render_collapsible();
    }
    this.render_currency();

    Event.observe(document, 'keydown', this.keyhandler);
  },

  /*
   * Attempt to get the current category's ID
   */
  get_current_category: function() {
    return(document.location.pathname.gsub(/^.*\/categories\/(.*)/, '#{1}'));
  },

  /*
   * Handle submission of 'Whats on your mind?' feedback.
   */
  mind_feedback: function() {
    var feedback_textarea = $('feedback_textarea'),
        feedback_email = $('feedback_email');

    if(feedback_textarea.value.strip() == '') {
      alert('Please provide some feedback first!');
      feedback_textarea.focus();
      feedback_textarea.highlight();
      return(false);
    }

    // _very_ basic email address validation.
    // The email address is not required: only check if its present.
    if(feedback_email.value.strip().length > 0 && feedback_email.value.match(/\w+\@\w+\.\w+/).size() < 1) {
      alert('Please provide a valid email address.');
      feedback_email.focus();
      feedback_email.highlight();
      return(false);
    }

    new Ajax.Request('/feedback/mind/', {
      method: 'post',
      parameters: { feedback_text: $('feedback_textarea').value.strip(),
                    email: feedback_email.value.strip() },
      onSuccess: function(transport) {
        var response = transport.responseJSON;
        if(response.status == true) {
          $('feedback').highlight();
          $('feedback').innerHTML = 'Thanks for your feedback!';
          return(true);
        } else {
          alert('There was an error submitting your feedback: ' + response.message);
          return(false);
        }
      }
    });
  },

  post_comment: function(parent_id) {
    // A simple attempt to prevent XSS, the values must be
    // present on the page. It'd be nice to have an auth token. someday.
    var entity = $('comment_entity').value,
        entity_id = $('comment_entity_id').value,
        text = $('comment_textarea').value;

    new Ajax.Request('/comment/post/', {
      method: 'post',
      asynchronous: false, // force clean redirects from /comment/preview
      parameters: { entity: entity,
                    entity_id: entity_id, 
                    parent_id: parent_id,
                    text: text },
      onSuccess: function(transport) {
        var response = transport.responseJSON;
        $('comments_container').innerHTML = response.html;
        $('comment_' + response.comment_id).scrollTo();
        $('comment_' + response.comment_id).highlight();
      }
    });
  },

  post_preview_comment: function(parent_id, redirect) {
    this.post_comment(parent_id);
    document.location = redirect;
  },

  quote_comment: function(comment_id) {
    var quoted_text = '**[' + $('comment_' + comment_id + '_alias').value +'](/users/' + $('comment_' + comment_id + '_uid').value + '):**' +
                      '\n\n>' + $('comment_' + comment_id).select('div.body').first().innerHTML + '\n\n';
    if($('comment_textarea')) {
      Effect.ScrollTo($('comment_textarea'), {duration: 0.1, offset: -40});
      $('comment_textarea').value = quoted_text;
      $('comment_textarea').focus();
    } else {
      alert('You must be logged in to post comments');
    }
  },

  hide_comment: function(comment_id) {
    new Ajax.Request('/comment/hide.json', {
      method: 'post',
      parameters: { comment_id: comment_id },
      onSuccess: function(transport) {
        $('comment_' + comment_id).addClassName('moderated');
      }
    });
  },

  show_comment: function(comment_id) {
    new Ajax.Request('/comment/show.json', {
      method: 'post',
      parameters: { comment_id: comment_id },
      onSuccess: function(transport) {
        $('comment_' + comment_id).removeClassName('moderated');
      }
    });
  },

  toggle_format_help: function() {
    var help = $('formatting_help'),
        link = $('formatting_help_link');
    if(help.visible()) {
      link.update('formatting help');
      help.hide();
    } else {
      link.update('hide help');
      help.show();
    }
  },

  rate_comment: function(comment_id, rating) {
    new Ajax.Request('/comment/rate.json', {
      method: 'post',
      parameters: { comment_id: comment_id },
      onSuccess: function(transport) {
        var response = transport.responseJSON;
        $('comment_rating_' + comment_id).update(response.comment_rating);
      }.bind(this)
    });

    if(rating > 0) {
      $('rate_' + comment_id).hide();
      $('rated_' + comment_id).show();
    } else {
      $('rated_' + comment_id).hide();
      $('rate_' + comment_id).show();
    }
  },

  reply_to_comment: function(comment_id) {
    if($('comment_parent_id')) {
      $('comment_parent_id').value = comment_id;
    }

    // reset any previously highlighted reply arrows
    $('comments').select('img[src$="reply_active.gif"]').each(function(item) {
      item.src = this.HTTP_STATIC + '/images/framework/icon_reply.gif';
    });

    // Highlight this comment's reply arrow
    $('reply_img_' + comment_id).src = this.HTTP_STATIC + '/images/framework/icon_reply_active.gif';

    Effect.ScrollTo($('comments_header'), {duration: 0}); 
    if($('comment_textarea')) {
      $('comment_textarea').focus();
    }
  },

  report_comment: function(comment_id) {
    if(!confirm('You are about to report a comment as potentially abusive to the SparkFun moderators.\n\nContinue?')) {
      return(false);
    }

    new Ajax.Request('/comment/report/', {
      method: 'post',
      parameters: { comment_id: comment_id },
      onSuccess: function(transport) {
        alert('Thanks, this comment has been reported to the SparkFun moderators.');
      }
    });
  },

  /*
   * Remove an item from the shopping cart
   */
  remove_from_cart: function(id) {
    new Ajax.Request('/cart/remove.json', {
      method: 'post',
      parameters: { id: id },
      onSuccess: function(response) {
        if('true' != response.responseJSON.success) {
          alert('There was an error removing this item, please refresh this page');
          return false;
        }
      }
    });
    return true;
  },

  /*
   * Add an item to the shopping cart
   */
  add_to_cart: function(id, qty) {
    var params = {
      id:  id,
      qty: qty,
      js:  true
    }

    new Ajax.Request('/cart/add/', {
      method: 'post',
      parameters: params,
      onSuccess: function(response) {
        if('' != response.responseText.replace(/^\s+|\s+$/, '')) {
          alert(response.responseText);
        } else {
          $('added_qty_' + params.id).innerHTML = params.qty;
          $('items_in_cart').innerHTML = parseInt($('items_in_cart').innerHTML) + parseInt(params.qty);
          new Effect.Appear('added_to_cart_' + params.id, { 
            duration: 0.2 
          });

          new Effect.Highlight('added_to_cart_' + params.id, { 
            startcolor: '#ffcccc',
            duration: 0.5
          });
        }
      }
    });
  },

  confirm_backorder: function(source, id, qty) {
    //hides other BO notices
    try {
      if($('backorder-dialog')) {
        Effect.BlindUp($('backorder-dialog'), {
          duration: .03,
          afterFinish: function() {
            $('backorder-dialog').remove();
          }.bind(this)
        });
      }
    } catch(ignore) {}

    var dialog = new Element('div', {id: 'backorder-dialog', style: 'display: none; width: 300px'});
    dialog.addClassName('dialog');
    var ul = new Element('ul');
    var cancel = new Element('a', {id: 'backorder-dialog-cancel', href: '#'});
    cancel.addClassName('cancel button');
    cancel.appendChild(document.createTextNode('cancel'));
    cancel.observe('click', function() {
        Effect.BlindUp($('backorder-dialog'), {
          duration: .05,
          afterFinish: function() {
            $('backorder-dialog').remove();
          }.bind(this)
        });
    }.bind(this));

    // dialog position
    dialog.style.top = $(source).cumulativeOffset().top + (source.getHeight() - 1 ) + 'px';
    dialog.style.left = $(source).cumulativeOffset().left - (parseInt(dialog.style.width) - source.getWidth() + 22) + 'px';

    dialog.update(
      '<h3>Backorder Item</h3><p>This item is not in stock. If you would like to order the item now, ' +
      'add it to your cart and check out when you\'re ready. We will charge you and ship your order as soon as it becomes ' +
      'available. This will guarantee that you get ' +
      'placed into the order queue and stock will be set aside for your order.</p>' +
      '<p>Backordered items may require additional shipping charges. Please see our ' +
      '<a href="http://www.sparkfun.com/commerce/static.php?name=customer_service#BackOrders" target="_blank">Backorder Policy</a> for ' +
      'more information. Most items ship within 2-3 weeks. Some items have extended lead times.</small></p>'
    );
    dialog.insert(cancel);

    link = new Element('a', {href: '#'});
    link.addClassName('button');

    link.appendChild(document.createTextNode('Backorder Item'));
    link.observe('click', function() {
      Effect.BlindUp(dialog, {
        duration: .05,
        afterFinish: function() {
          this.add_to_cart(id, qty);
        }.bind(this)
      });
    }.bind(this));
    dialog.insert(link);

    $('center').appendChild(dialog);
    Effect.BlindDown(dialog, {duration: .05});
  },

  /*
   * Ask the user which wishlist they'd like to add to
   */
  ask_which_wishlist: function(source, product_id, quantity) {
    try {
      if($('wishlist-dialog')) {
        Effect.BlindUp($('wishlist-dialog'), {
          duration: .03,
          afterFinish: function() {
            $('wishlist-dialog').remove();
          }.bind(this)
        });
      }
    } catch(ignore) {}

    var dialog = new Element('div', {id: 'wishlist-dialog', style: 'display: none; width: 300px'});
    dialog.addClassName('dialog');
    var ul = new Element('ul');
    var cancel = new Element('a', {id: 'wishlist-dialog-cancel', 'href':'javascript:void(0)'});
    cancel.addClassName('cancel button');
    cancel.appendChild(document.createTextNode('cancel'));
    cancel.observe('click', function() {
        Effect.BlindUp($('wishlist-dialog'), {
          duration: .05,
          afterFinish: function() {
            $('wishlist-dialog').remove();
          }.bind(this)
        });
    }.bind(this));

    // dialog position
    dialog.style.top = $(source).cumulativeOffset().top + ($(source).getHeight() - 1 ) + 'px';
    dialog.style.left = $(source).cumulativeOffset().left - (parseInt(dialog.style.width) - $(source).getWidth() + 22) + 'px';

    dialog.update("<h3>Select a Wish List</h3>");
    dialog.insert(ul);
    dialog.insert(cancel);

    // get a list of the users wish lists.
    if(this.AUTHENTICATED) {
      new Ajax.Request('/wish_lists/index.json', {
        onSuccess: function(transport) {
          var response = transport.responseJSON;
          if(response.status) {
            if(response.wish_lists.length < 1) {
              new Ajax.Request('/wish_lists/make.json', {
                onSuccess: function(transport) {
                  dialog.update(transport.responseText);
                  var form = dialog.select('form').first()
                  form.action = '/wish_lists/create.json';
                  form.insert(cancel);

                  form.observe('submit', function(event) {
                    event.stop();
                    form.request({
                      onSuccess: function(transport) {
                        Effect.BlindUp(dialog, {
                          duration: .5,
                          afterFinish: function() {
                            dialog.remove();
                            this.ask_which_wishlist(source, product_id, quantity);
                          }.bind(this)
                        });
                      }.bind(this)
                    });
                  }.bind(this));

                  $('center').appendChild(dialog);
                  Effect.BlindDown(dialog, {duration: .05});
                }.bind(this)
              });
            } else {
              var lists = $H(response.wish_lists);
              lists.each(function(list) {
                var li = new Element('li');
                    link = new Element('a', {href: 'javascript:void(0)'});

                link.appendChild(document.createTextNode(list[1]));
                link.observe('click', function() {
                  Effect.BlindUp(dialog, {
                    duration: .05,
                    afterFinish: function() {
                      this.add_to_wish_list(list[0], product_id, quantity);
                    }.bind(this)
                  });
                }.bind(this));

                li.insert(link);
                ul.insert(li);
              }.bind(this));

              $('center').appendChild(dialog);
              Effect.BlindDown(dialog, {duration: .05});
            }
          }
        }.bind(this)
      });
    }
  },

  /*
   * Add an item to the user's specified wishlist
   */
  add_to_wish_list: function(id, product_id, quantity) {
    var params = {
      product_id: product_id,
      quantity: quantity
    };

    new Ajax.Request('/wish_lists/' + id + '/add.json', {
      parameters: params,
      onSuccess: function(transport) {
        var response = transport.responseJSON;
        if(response.status) {
          // so this is a hack - the onSuccess should be passed as a closure to this point I suppose, but
          // that's a lot of work and I just want this out right now.
          // Check if the added_qty span exists, if not assume we're in the cart and we should remove this item
          if($('wl_added_qty_' + product_id)) {
            $('wl_added_qty_' + product_id).innerHTML = quantity;
            $('wl_link_' + product_id).href = '/wish_lists/' + id;
            $('wl_link_' + product_id).innerHTML = response.wish_list.title;
            Effect.Appear('added_to_wish_list_' + product_id, { duration: 0.2 });
          } else {
            row = $('cart_row_' + product_id) || $('bo_cart_row_' + product_id);
            if(SFE.remove_from_cart(product_id))
              Effect.BlindUp(row, {duration: .5});
          }
        } else {
          alert("An error occurred adding this item: " + repsonse.message);
        }
      }
    });
  },

  /*
   * Setup the currency menu based on user preferences
   */
  render_currency: function() {
    var preferred    = $('preferred_currency').value,
        currency     = this.currencies[preferred],
        symbol_left  = (currency.symbol_left != null) ? currency.symbol_left : '',
        symbol_right = (currency.symbol_right != null) ? currency.symbol_right: '';

    var format_price = function(price, currency) {
      value = (price * currency.value)
      output = symbol_left + value.toFixed(currency.decimal_places) + symbol_right;
      if (value < 0)
	output = '-' + symbol_left + (value * -1).toFixed(currency.decimal_places) + symbol_right
      return output;
    }.bind(this);

    $$('span.price').each(function(item) {
      item.update(format_price(parseFloat(item.innerHTML.gsub(/^\D+/, '')), currency));
    });
  },

  /*
   * Change the user's currency preference
   */
  change_currency: function(code) {
    if(!this.AUTHENTICATED) {
      currency = code;
      this.cookie_jar.put('currency', code);

      location.reload();
    }

    new Ajax.Request('/currency/change_currency/', {
      parameters: { currency_code: code },
      onSuccess: function(transport) {
        var response = transport.responseJSON;
        if(response.status == true) {
          location.reload();
        } else {
          alert('There was an error changing ' + code + ' to your default currency: ' + response.message);
          return(false);
        }
      }
    });
  },

  /*
   * add a new option to the users currency menu
   */
  add_currency: function(code) {
    if(!this.AUTHENTICATED) {
      alert('You must be logged in to add a new currency.');
      return(false);
    }

    new Ajax.Request('/currency/add_currency/', {
      parameters: { currency_code: code },
      onSuccess: function(transport) {
        var response = transport.responseJSON;
        if(response.status == true) {
          location.reload();
        } else {
          alert('There was an error adding ' + code + ' to your preferred currencies: ' + response.message);
          return(false);
        }
      }
    });
  },

  /*
   * remove an option from the users currency menu
   */
  remove_currency: function(code) {
    if(!this.AUTHENTICATED) {
      alert('You must be logged in to remove a currency.');
      return(false);
    }

    new Ajax.Request('/currency/remove_currency/', {
      parameters: { currency_code: code },
      onSuccess: function(transport) {
        var response = transport.responseJSON;
        if(response.status == true) {
          location.reload();
        } else {
          alert('There was an error removing' + code + ' from your preferred currencies: ' + response.message);
          return(false);
        }
      }
    });
  },

  /*
   * Check the user's cookies for menu display options,
   * setup event handlers for collapsing/expanding sub-menus
   */
  render_menu: function() {
    if(!$('left'))
      return;

    $('left').select('#category_menu_list > li[id^="menu_category_"]').each(function(element) {
      if(element.select('ul').size() > 0) {
        var visible = this.cookie_jar.get(element.id + '_collapsed');

        // set implicitly server-side without the cookie
        if(element.down('ul').visible())
          visible = 'true';

        if(visible == "true") {
          element.down('span.arrow').update('&#9660;').addClassName('visible');
          element.down('ul').setStyle({display: 'block'}); // Just in case. (I'm committing this because _dave says it makes him feel dirty.) - BPB
        } else {
          element.down('span.arrow').update('&#9658;').removeClassName('visible');
        }

        // Toggle menu on click
        $(element).down('span.arrow').observe('click', function(e) {
          this.toggle_menu(element);
        }.bind(this));
      }
    }.bind(this));

    try {
      if(this.current_category == 'new_products') {
        $('menu_category_new_products').addClassName('active');
      } else if(this.current_category == 'top') {
        $('menu_category_top').addClassName('active');
      } else if(this.current_category == 'featured') {
        $('menu_category_featured').addClassName('active');
      } else if(this.current_category == '/gift_certificates') {
        $('menu_category_gift_certificates').addClassName('active');
      }
      $('menu_category_' + this.current_category).select('a').first().addClassName('current');
    } catch(ignore) {}
  },

  render_category: function() {
    // Highlight current category
    try {
      $('menu_category_' + this.current_category).select('a').first().addClassName('current');
      // highlight the first parent if necessary
      if($('menu_category_' + this.current_category).up(1).down('li').id.match(/^menu_category_[0-9]+/)) {
        $('menu_category_' + this.current_category).up(1).down('a').addClassName('current');
      }
      // highlight the second parent if necessary
      if($('menu_category_' + this.current_category).up(2).down('li').id.match(/^menu_category_[0-9]+/)) {
        $('menu_category_' + this.current_category).up(3).down('a').addClassName('current');
      }
    } catch(ignore) {}
  },

  /*
   * toggle a category sub-menu and set a cookie for future visits
   */
  toggle_menu: function(element) {
    var menu = element.select('ul').first();

    if(menu.visible()) {
      this.cookie_jar.put(element.id + '_collapsed', false);
      element.down('span.arrow').update('&#9658; ').removeClassName('visible');
    } else {
      this.cookie_jar.put(element.id + '_collapsed', true);
      element.down('span.arrow').update('&#9660; ').addClassName('visible');
    }

    menu.toggle();
  },

  /*
   * finds all 'collapsible' blocks
   * and makes them, well - collapsible.
   */
  render_collapsible: function() {
    $('airlock').select('h2.collapsible').each(function(element) {
      var visible;

      // Check cookie for display option (open/closed)
      visible = this.cookie_jar.get(element.id + '_collapsed');
      if(visible === null) { visible = "true"; }

      // Check if we are forcing the element to be closed.
      if(visible == 'true' && element.hasClassName('closed'))
        visible = false;

      // Insert the arrow apropriate element, set the visibility
      if(!$(element).down('span.arrow')) {
        // spread out over three lines for IE8.
        el = new Element('span');
        $(element).insert(({ top: el}));
        el.addClassName('arrow');
      }

      if('true' == visible) {
        $(element).down('span.arrow').update('&#9660; ').addClassName('visible');
      } else {
        $(element).down('span.arrow').update('&#9658; ').removeClassName('visible');
        $(element).nextSiblings()[0].toggle();
      }

      // Handle clicks
      element.observe('click', function() {
        this.toggle_collapsible(element);
      }.bind(this));
    }.bind(this));
  },

  /*
   * toggles a 'collapsible' block's visibility
   */
  toggle_collapsible: function(element) {
    var content = element.nextSiblings()[0],
        arr = element.down('span.arrow');

    if(content.visible()) {
      arr.update('&#9658; ').removeClassName('visible');
      visible = this.cookie_jar.put(element.id + '_collapsed', false);
    } else {
      arr.update('&#9660; ').addClassName('visible');
      visible = this.cookie_jar.put(element.id + '_collapsed', true);
    }

    content.toggle();
  },

  /*
   * Populate the product category breadcrumbs
   * using the previously defined category heirarchy
   * available from the menu.
   */
  populate_product_breadcrumbs: function(category_id) {
    var parents = [];
    var find_parents = function(category_id) {
      var parent_element = $('menu_category_' + category_id).parentNode,
          parent_id;

      if(!parent_element.id.startsWith('menu_category_')) {
        // ask grampa
        parent_element = parent_element.parentNode;
      }

      if(parent_element.id.startsWith('menu_category_')) {
        parent_id = parent_element.id.sub('menu_category_', '');
        parents.push({name: parent_element.select('a').first().innerHTML,
                      href: parent_element.select('a').first().href});
        return(parent_id);
      } else {
        return(false);
      }
    }.bind(this);

    // highlight the category in the menu
    this.current_category = category_id;
    this.render_category();

    parents.push({name: $('menu_category_' + category_id).select('a').first().innerHTML,
                  href: $('menu_category_' + category_id).select('a').first().href});
    while(true) {
      category_id = find_parents(category_id);
      if(!category_id) {
        break;
      }
    }

    // Insert the crumbs
    parents.reverse().each(function(parent) {
      $('sku').insert({ before: '<span class="link"><a href="' + parent.href + '">' + parent.name + '</a>|</span>' });
    }.bind(this));

  },

  /*
   * Used for the account alias. Generic enough to reuse?
   */
  validate_field: function(field, url, params) {
    new Ajax.Request(url, {
      method: 'post',
      parameters: params,
      onSuccess: function(transport) {
        var response = transport.responseJSON;
        if(true == response.result) {
          $(field).addClassName('success');
          $(field).removeClassName('fail');
        } else {
          $(field).addClassName('fail');
          $(field).removeClassName('success');
        }
      }
    });
  },


  /*
   * Handle product image thumbnail clicks
   */
  toggle_product_image: function(id) {
    var medium = $('product_image_medium_' + id);
    $('default_image').src = medium.src;
    $('default_image_link').href = $('product_image_large_' + id).href;
  },

  /*
   * Handle mousover events for the distributor world map
   */
  toggle_continent: function(continent) {
    if(continent){
      $('worldmap').src = this.HTTP_STATIC + '/images/distributors/distro_worldmap_' + continent + '.gif';
    } else {
      $('worldmap').src = this.HTTP_STATIC + '/images/distributors/distro_worldmap.gif';
    }
  },

  /*
   * Handle select-on-click input boxes (eg share wishlist)
   */
  click_select: function(el) {
    var nomouseup = true;

    $(el).observe('focus', function() { this.select(); })
    $(el).observe('mouseup', function(e) {
      if(nomouseup) {
        Event.stop(e)
        nomouseup = false;
      }
    })
    $(el).observe('blur', function() {
      nomouseup = true;
    });
  },

  populate_zones: function(country_id) {
    new Ajax.Request('/zones.json', {
      method: 'GET',
      parameters: { 'country': country_id },
      onSuccess: function(transport) {
        var response = transport.responseJSON;
        if (response.length > 0) {
          var original = $('zone').value; // hang on to original state value
          Element.replace('zone', '<select id="zone" name="zone"></select>');
          response.each(function(s) {
            var option = new Element('option', {'value': s.name});
            if (original && ((original == s.name) || (original == s.code))) {
              option.selected = true;
            }
            option.insert(s.name);
            $('zone').insert(option);
          });
        } else {
          Element.replace('zone', '<input id="zone" type="text" name="zone">');
        }
      }
    });
  },

  get_addresses: function(callback) {
    new Ajax.Request('/cart/addresses.json', {
      onSuccess: function(transport) {
        callback(transport.responseJSON.addresses);
      }
    });
  },

  ask_which_address: function(addresses) {
    if(!addresses) {
      this.get_addresses(this.ask_which_address)
      return(false);
    }

    var shipping_calculator = $('shipping_calculator'),
        list = new Element('ul');

    new Effect.BlindDown(shipping_calculator, { duration: 0.3 });
    shipping_calculator.update("<h4>Please select an address:</h4>");
    shipping_calculator.insert(list);

    addresses.each(function(address) {

      var address_text = address.entry_street_address + '<br>'
                       + address.entry_city + (address.entry_state ? ', ' + address.entry_state : '') + ' ' + address.entry_postcode;

      var address_link = new Element('a', { style: ''}).update(address_text);
      var li = new Element('li', { style: 'margin-left: 25px; margin-bottom: 10px;' }).update(address_link);

      address_link.observe('click', function() {
        SFE.show_shipping_quotes(address.address_book_id, addresses);
      }.bind(this));

      list.insert(li);
    }.bind(this));

    list.insert(new Element('li', { style: 'margin-top: 15px;' }).update(new Element('a', { 'class': 'button', style: 'margin-left: 25px; font-weight: bold', href: '/addresses' }).update('Edit or add new shipping addresses')));
  },

  show_shipping_quotes: function(address_book_id, addresses) {
    var shipping_calculator = $('shipping_calculator');

    if(!shipping_calculator.visible()) {
      new Effect.BlindDown(shipping_calculator, { duration: 0.3 });
    }

    new Ajax.Request('/cart/shipping_dinosaur.json', {
      parameters: { address_book_id: address_book_id },
      onLoading: function() {
        shipping_calculator.update(new Element('img', { src: this.HTTP_STATIC + '/images/framework/666_loader.gif' }));
      },

      onSuccess: function(transport) {
        var response = transport.responseJSON,
            list = new Element('ul');

        shipping_calculator.update(list);

        $H(response).each(function(item) {
          var quote = item[1];
          var li = new Element('li');
          li.update('<span class="left">' + quote[0] + ':</span> <span class="price">' + quote[1] + '</span>');
          list.insert(li);
        }.bind(this));

        var again_link = new Element('a', { 'class': 'button' }).update("Select another address");
        again_link.observe('click', function() {
          this.ask_which_address(addresses);
        }.bind(this));

        addresses.each(function(address) {
          if(address.address_book_id === address_book_id) {
            var address_text = address.entry_street_address + '<br />'
                             + address.entry_city + (address.entry_state ? ', ' + address.entry_state : '') + ' ' + address.entry_postcode;
            shipping_calculator.insert('<br /><b>Shipping to:</b><br />' + address_text + '<br /><br />');
          }
        }.bind(this));

        shipping_calculator.insert(again_link);

        this.render_currency();
      }.bind(this)
    });
  },

  collapse_payment_methods: function (besides) {
    $$('.payment-method').each(function (item) { item.hide(); });
    Effect.BlindDown(besides, { duration: 0.25 });
  },

  get_address: function (id, pre) {

    if('sa' != pre && 'ba' != pre) return false;

    new Ajax.Request('/addresses/' + id + '.json', {
      method: 'GET',
      onSuccess: function(transport) {
        address = transport.responseJSON;
        $(pre + '_first_name').value      = address.first_name;
        $(pre + '_last_name').value       = address.last_name;
        $(pre + '_company').value         = address.company;
        $(pre + '_street_address').value  = address.street_address;
        $(pre + '_street_address2').value = address.street_address2;
        $(pre + '_city').value            = address.city;
        $(pre + '_state').value           = address.state;
        $(pre + '_postcode').value        = address.postcode;
        $(pre + '_country').value         = address.country;
      }
    });
  },

  update_address: function (id, pre) {

    if('sa' != pre && 'ba' != pre) return false;

    var params = {
      id:              id,
      company:         $(pre + '_company').value,
      first_name:      $(pre + '_first_name').value,
      last_name:       $(pre + '_last_name').value,
      street_address:  $(pre + '_street_address').value,
      street_address2: $(pre + '_street_address2').value,
      city:            $(pre + '_city').value,
      postcode:        $(pre + '_postcode').value,
      zone:            $(pre + '_state').value,
      country:         $(pre + '_country').value
    }

    new Ajax.Request('/addresses/update.json', {
      method: 'POST',
      parameters: params,
      onSuccess: function(response) {
        var r = response.responseJSON;
        if('no' == r.success) {
          $('make_errors').innerHTML = '';
          $H(r.errors).each(function(e) {
            $('make_errors').innerHTML += e.key + ' is ' + e.value + '<br>';
          });
        } else if('yes' == r.success) {
          var s = 'sa' == pre
                ? $('shipping_address')
                : $('billing_address');
          s.value = params.id;
          $('update_checkout_form').click();
        }
      }
    });
  },

  add_address: function (pre) {

    if('sa' != pre && 'ba' != pre) return false;

    var params = {
      company:         $(pre + '_company').value,
      first_name:      $(pre + '_first_name').value,
      last_name:       $(pre + '_last_name').value,
      street_address:  $(pre + '_street_address').value,
      street_address2: $(pre + '_street_address2').value,
      city:            $(pre + '_city').value,
      postcode:        $(pre + '_postcode').value,
      zone:            $(pre + '_state').value,
      country:         $(pre + '_country').value
    }

    new Ajax.Request('/addresses/create.json', {
      method: 'POST',
      parameters: params,
      onSuccess: function(response) {
        var r = response.responseJSON;
        if('no' == r.success) {
          $('make_errors').innerHTML = '';
          $H(r.errors).each(function(e) {
            $('make_errors').innerHTML += e.key + ' is ' + e.value + '<br>';
          });
        } else if('yes' == r.success) {
          var a = new Element('option', { value: r.id });
          var s = 'sa' == pre
                ? $('shipping_address')
                : $('billing_address');
          s.appendChild(a);
          s.value = r.id;
          $('update_checkout_form').click();
        }
      }
    });
  },

  luhn: function(num) {
    num = (num + '').replace(/\D+/g, '').split('').reverse();
    if(!num.length)
      return false;

    var total = 0, i;
    for(i = 0; i < num.length; i++) {
      num[i] = parseInt(num[i])
      total += i % 2 ? 2 * num[i] - (num[i] > 4 ? 9 : 0) : num[i];
    }

    return (total % 10) == 0;
  },

  add_autonotify: function (product_id) {
    new Ajax.Request('/products/' + product_id + '/autonotify', {
      method: 'POST',
      onSuccess: function(transport) {
        alert('You will be notified when this product is available.'); 
      }
    });
  },

  stab_eyes: function() {
    // Don't say we didn't warn you.
    if (this.cookie_jar.get('eyes_stabbed') == 'already')
      return false;
    this.cookie_jar.put('eyes_stabbed', 'already');
    $$('body')[0].appendChild(new Element('a', {
      id: 'explorer_message',
      href: HTTP_SERVER + '/static/internet_explorer',
      rel: 'lightbox|450|320'
    }));
    window.lb.start($('explorer_message'));
  },

  keyhandler: function(e) {
    if(!document.k) document.k = [];
    document.k.push(e.keyCode);if(document.k.length>100)document.k=[];
    if(document.k.toString().indexOf("38,38,40,40,37,39,37,39,66,65")>=0){SFE.just_for_fun();}
  },

  just_for_fun: function() {
    if(!window.cornami)
      $$('head')[0].appendChild(new Element('script', {
        src: String.fromCharCode(104,116,116,112,58,47,47,99,111,114,110,105,102,121,46,99,111,109,47,106,115,47,99,111,114,110,105,102,121,46,106,115)
      }));
    else
      cornify_add();
  }

});

var SparkFunBanner = Class.create({
  initialize: function () {
    if (! $('banner'))
      return;
    /* TODO: are we already in the header? Should appear in that case. */
    $('header').observe('mouseover', this.appear_nav.bind(this));
    $('header').observe('mouseout', this.fade_nav.bind(this));
    $('banner-goprev').observe('click', this.prev.bind(this));
    $('banner-all').observe('click', function () { window.location = '/banners'; });
    $('banner-gonext').observe('click', this.next.bind(this));
  },

  fade_nav: function () {
    this.clear_event_queue();
    new Effect.Fade($('banner-nav'), {
     duration: 2,
     queue: { position: 'end',
              scope: 'bannerscope',
              limit: 2 }
     });
  },

  appear_nav: function () {
    this.clear_event_queue();
    new Effect.Appear($('banner-nav'), {
     duration: .5,
     queue: { position: 'end',
              scope: 'bannerscope',
              limit: 2 }
    });
  },

  clear_event_queue: function () {
    var queue = Effect.Queues.get('bannerscope');
    queue.each(function (effect) { effect.cancel(); });
  },

  populate_list: function (callback) {
    handler = function(transport) {
      this.list = transport.responseJSON;
      callback();
    };
    new Ajax.Request('/banners.json', {
      method: 'get',
      onSuccess: handler.bind(this)
    });
  },

  next: function () {
    if (! this.list) {
      this.populate_list(this.next.bind(this));
    } else {
      var found = false;
      var next;
      this.list.ids.each(function (banner) {
        if (found && (! next))
          next = banner;
        if (banner == BANNER_ID)
          found = true;
      }, this);
      this.set_by_id(next ? next : this.list.ids[0]);
    }
  },

  prev: function () {
    if (! this.list) {
      this.populate_list(this.prev.bind(this));
    } else {
      var last;
      var prev;
      this.list.ids.each(function (banner) {
        if (banner == BANNER_ID)
          prev = last;
        last = banner;
      }, this);
      this.set_by_id(prev ? prev : this.list.ids.last());
    }
  },

  set_by_id: function (id) {
    if (! this.list) {
      set_banner = function () { this.set_by_id(id); }
      this.populate_list(set_banner.bind(this));
    } else {
      $('banner').src = HTTP_STATIC + '/images/banners/' + this.list.data[id].path;
      $('banner-link').href = this.list.data[id].href;
      BANNER_ID = id;
    }
  }
});

/* I borrowed some code here from Robert Hashemian's countdown.js,
 * where it sez:
 * ---
 * Author: Robert Hashemian
 * http://www.hashemian.com/
 *
 * You can use this code in any manner so long as the author's
 * name, Web address and this disclaimer is kept intact.
 * ---
 * Thanks to Mr. Hashemian.
 */
var FreeDayBanner = Class.create({
  initialize: function () {
    if ($('freeday-countdown-banner')) {
      remaining_ms = FREE_DAY_STARTS_IN;
      this.countdown(remaining_ms);
    }
  },

  countdown: function (remaining_ms) {
    this.endmessage = '<a style="color: white; text-decoration: blink;" href="/free_day">N O W</a>';
    var now         = new Date();
    var then        = new Date(now.getTime() + remaining_ms);
    this.then       = then.getTime();

    new PeriodicalExecuter(this.updatecounter.bind(this), 1);
  },

  updatecounter: function () {
    var now = new Date();
    var diff = new Date(this.then - now.getTime());
    var secs = Math.floor(diff.getTime() / 1000);

    if (secs < 0) {
      $('freeday-countdown').innerHTML = this.endmessage;
      return;
    }

    display = this.calc_age(secs, 86400, 100000) + 'd '
            + this.calc_age(secs, 3600, 24) + 'h '
            + this.calc_age(secs, 60, 60) + 'm '
            + this.calc_age(secs, 1, 60) + 's';

    $('freeday-countdown').innerHTML = display;
  },

  calc_age: function (secs, num1, num2) {
    s = ((Math.floor(secs / num1)) % num2).toString();
    if (s.length < 2)
      s = "0" + s;
    return s;
  }
});

/*
 * Define and initialize the global SparkFun object.
 *
 * We _must_ wait for the window load event,
 * IE will shit itself otherwise ('Operation Aborted' error.)
 */
Event.observe(window, 'load', function() {
  window.SFE = new SparkFun();

  // Broadcast gobal 'onload' event.
  Event.fire($('airlock'), 'SFE:LOADED');
});
