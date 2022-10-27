package ru.itmo.web.hw4.util;

import ru.itmo.web.hw4.model.*;

import javax.servlet.http.HttpServletRequest;
import javax.sql.PooledConnection;
import java.util.Arrays;
import java.util.List;
import java.util.Map;

public class DataUtil {
    private static final List<User> USERS = Arrays.asList(
            new User(1, "MikeMirzayanov", "Mike Mirzayanov"),
            new User(6, "pashka", "Pavel Mavrin"),
            new User(7, "Daerys", "Ivan Vikulaev"),
            new User(9, "geranazavr555", "Georgiy Nazarov"),
            new User(11, "tourist", "Gennady Korotkevich"),
            new User(13, "shtoorik", "Yaroslav Sakhno")
    );

    public static final List<Post> POSTS = Arrays.asList(
            new Post(1, "Codeforces Round #830 (Div. 2)", "Hello, Codeforces!\n" +
                    "\n" +
                    "I am happy to invite you to my Codeforces Round #830 (Div. 2) which will be held at Sunday, October 23, 2022 at 13:05. The round will be rated for all the participants with rating strictly less than 2100 before Sunday, October 23, 2022 at 10:50.\n" +
                    "\n" +
                    "The tasks were created and prepared by 74TrAkToR. I would like to thank everyone who helped me a lot with round preparation.\n" +
                    "\n" +
                    "Coordinator Artyom123 for excellent communication and help with preparation.\n" +
                    "Testers gisp_zjz, Pointy, q-w-q-w-q, feecIe6418, nnv-nick, xiaoziya, Ormlis, huangzirui, Chenyu_Qiu, triple__a, gyh20, low_, voventa, tibinyte, Mr.Robot_28, Qingyu, RUSH_D_CAT, physics0523, Lucina, Rhodoks, Tekor, Milesian, Kalashnikov, Lokeo, mejiamejia, TomiokapEace, Chenyu_Qiu, AquaMoon, flowerletter for high-quality testing and valuable advices.\n" +
                    "As well as the testers who tested the CheReKOSH Team Olympiad — it was from the problems of this Olympiad that the round was composed: orz, DIvanCode, CAMOBAP31.\n" +
                    "MikeMirzayanov for amazing Codeforces and Polygon platforms.\n" +
                    "During the round you will need to solve 5 problems, some of which have subtasks. You will have 2 hours to solve them.\n" +
                    "\n" +
                    "Score distribution will be announced shortly before the round.\n" +
                    "\n" +
                    "We wish you good luck and high rating!\n" +
                    "\n" +
                    "UPD: Score distribution: 750−750−(1000−1000)−(1250−1250)−3000\n" +
                    "UPD: Editorial", 1),
            new Post(2, "Educational Codeforces Round 137 [Rated for Div. 2]", "Hello Codeforces!\n" +
                    "\n" +
                    "On Monday, October 17, 2022 at 17:35 Educational Codeforces Round 137 (Rated for Div. 2) will start.\n" +
                    "\n" +
                    "Series of Educational Rounds continue being held as Harbour.Space University initiative! You can read the details about the cooperation between Harbour.Space University and Codeforces in the blog post.\n" +
                    "\n" +
                    "This round will be rated for the participants with rating lower than 2100. It will be held on extended ICPC rules. The penalty for each incorrect submission until the submission with a full solution is 10 minutes. After the end of the contest you will have 12 hours to hack any solution you want. You will have access to copy any solution and test it locally.\n" +
                    "\n" +
                    "You will be given 6 or 7 problems and 2 hours to solve them.\n" +
                    "\n" +
                    "The problems were invented and prepared by Ivan BledDest Androsov, Alex fcspartakm Frolov and me. Also huge thanks to Mike MikeMirzayanov Mirzayanov for great systems Polygon and Codeforces.\n" +
                    "\n" +
                    "The round is based on the Qualification stage of the Southern and Volga Russian Regional Contest. Thus, we kindly ask its participants to avoid participating in the round.\n" +
                    "\n" +
                    "Good luck to all the participants!\n" +
                    "\n" +
                    "UPD: Editorial is out", 11),
            new Post(3, "КредI\\iтный путь на КТ", "Канал создан фанатами Ярослава - людьми с КТ.\n" +
                    "\n" +
                    "Последние новости: Ярослав снова учится на КТ.\n" +
                    "\n" +
                    "Люди, замеченные в хейте, будут баниться в канале >:(" +
                    "https://t.me/yaroslav_sakhno", 13)
    );

    public static void addData(HttpServletRequest request, Map<String, Object> data) {
        data.put("users", USERS);
        data.put("posts", POSTS);

        for (User user : USERS) {
            if (Long.toString(user.getId()).equals(request.getParameter("logged_user_id"))) {
                data.put("user", user);
            }
            for (Post post : POSTS) {
                if (user.getId() == post.getUser_id()) {
                    user.incPostCount();
                }
            }
        }
    }
}
