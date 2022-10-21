package ru.itmo.web.hw4.web;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpServlet;
import java.io.File;
import java.io.IOException;
import java.io.OutputStream;
import java.nio.file.Files;
import java.nio.file.Paths;


public class StaticServlet extends HttpServlet {
    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response) throws IOException {
        String[] uris = request.getRequestURI().split("\\+");
        String path = getServletContext().getRealPath(".");
        if (!path.endsWith("src\\main\\webapp")) {
            path += "/../../src/main/webapp";
        }

        for (String uri : uris) {
            if (!uri.startsWith("/")) {
                uri = "/" + uri;
            }

            if (!Files.exists(Paths.get(path, uri))) {
                if (!Files.exists(Paths.get(getServletContext().getRealPath(uri)))) {
                    response.sendError(HttpServletResponse.SC_NOT_FOUND);
                    return;
                }
            }

        }

        for (String uri : uris) {

            File file = new File(path, uri);

            if (!file.isFile()) {
                file = new File(getServletContext().getRealPath(uri));
            }

            try (OutputStream outputStream = response.getOutputStream()) {
                Files.copy(file.toPath(), outputStream);
                outputStream.flush();
            }
        }
    }
}
