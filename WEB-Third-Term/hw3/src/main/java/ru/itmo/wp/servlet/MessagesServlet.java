package ru.itmo.wp.servlet;

import com.google.gson.Gson;

import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;

public class MessagesServlet extends HttpServlet {

    List<Messages> messages = new ArrayList<>();

    @Override
    protected void doPost(HttpServletRequest request, HttpServletResponse response) throws IOException {
        response.setContentType("application/json");
        response.setCharacterEncoding("UTF-8");
        String uri = request.getRequestURI();
        HttpSession session = request.getSession();

        switch (uri) {
            case "/message/auth":
                response.setContentType("application/json");
                String name = request.getParameter("user");
                if (name == null) {
                    name = (String) session.getAttribute("user");
                } else {
                    session.setAttribute("user", name);
                }
                if (name != null && name.trim().isEmpty()) {
                    break;
                }
                printJSON(name, response);
                break;
            case "/message/findAll":
                if (session.getAttribute("user") == null) {
                    printJSON(null, response);
                    break;
                }
                printJSON(messages, response);
                break;
            case "/message/add":
                String text = request.getParameter("text");
                if (!text.trim().isEmpty()) {
                    messages.add(new Messages(session.getAttribute("user").toString(), text));
                }
                break;
            default:
                response.sendError(HttpServletResponse.SC_NOT_FOUND);
                break;
        }
    }

    private void printJSON(Object objectToConvert, HttpServletResponse response) throws IOException {
        String json = new Gson().toJson(objectToConvert);
        response.getOutputStream().write(json.getBytes(StandardCharsets.UTF_8));
    }

    private static class Messages {
        private final String user, text;

        public Messages (String user, String text) {
            this.user = user;
            this.text = text;
        }

        public String getUser() {
            return user;
        }

        public String getText() {
            return text;
        }
    }

}
