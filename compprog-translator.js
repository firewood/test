var TOKEN = "xoxp-YOUR_TOKEN_HERE";

function doPost(e) {
  try {
    var json = JSON.parse(e.postData.getDataAsString());
    if (json.type == "url_verification") {
      return ContentService.createTextOutput(json.challenge);
    }
    // https://api.slack.com/events/reaction_added
    // scope: "reactions:read"
    if (json.type == "event_callback" && json.event.type == "reaction_added") {
      return ContentService.createTextOutput(onReactionAdded(json.event));
    }
  } catch (ex) {

  }
}

function postDebugMessage(json) {
  channel = "#__debug";
  text = JSON.stringify(json);
  UrlFetchApp.fetch("https://slack.com/api/chat.postMessage?token=" + TOKEN + "&channel=" + encodeURIComponent(channel) + "&text=" + encodeURIComponent(text));
}

// https://api.slack.com/methods/chat.postMessage
// scope: "chat:write:user" or "chat:write:bot"

function postThreadMessage(channel, ts, text) {
  if (channel && ts && text) {
    UrlFetchApp.fetch("https://slack.com/api/chat.postMessage?token=" + TOKEN + "&channel=" + channel + "&thread_ts=" + ts + "&text=" + encodeURIComponent(text));
  }
}

// https://api.slack.com/methods/conversations.replies
// "channels:history" or "groups:history"

function getMessages(channel, ts) {
  var response = UrlFetchApp.fetch("https://slack.com/api/conversations.replies?token=" + TOKEN + "&channel=" + channel + "&ts=" + ts);
  var json = JSON.parse(response.getContentText());
  return json.messages;
}

function isTranslated(messages, lang) {
  for (var i in messages) {
    var message = messages[i];
    if (message.text.substring(0, lang.length) == lang) {
      return true;
    }
  }
  return false;
}

function onReactionAdded(json) {
//  postDebugMessage(json);
  var channel = json.item.channel;
  var type = json.item.type;
  var ts = json.item.ts;
  var reaction = json.reaction;
  if (type == "message") {
    var messages = getMessages(channel, ts);
//    postDebugMessage(messages);
    if (messages) {
      var message = messages[0].text;
      var languagePrefix;
      var translateTo;
      if ((reaction == "us" || reaction == "flag-us") && !isTranslated(messages, ":us:")) {
        languagePrefix = ":us:";
        translateTo = "en";
      }
      if ((reaction == "gb" || reaction == "flag-gb") && !isTranslated(messages, ":gb:")) {
        languagePrefix = ":gb:";
        translateTo = "en";
      }
      if ((reaction == "jp" || reaction == "flag-jp") && !isTranslated(messages, ":jp:")) {
        languagePrefix = ":jp:";
        translateTo = "ja";
      }
      if (translateTo) {
        var translatedMessage = languagePrefix + " " + LanguageApp.translate(message, "", translateTo);
        postThreadMessage(channel, ts, translatedMessage);
      }
    }
  }
  return "OK";
}
