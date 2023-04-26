import pygame
import MyVector as mv  # vector 클래스

rgb = {
    "BLACK": (0, 0, 0),
    "WHITE": (255, 255, 255),
    "BLUE": (0, 0, 255),
    "GREEN": (0, 255, 0),
    "RED": (255, 0, 0),
    "YELLOW": (255, 255, 0),
}


class Actor:
    def __init__(self, x, y):
        self.pos = mv.MyVector(x, y)
        self.name = ""
        self.skill = ""
        self.quest = ""

    def set_pos(self, x, y):
        self.pos.x = x
        self.pos.y = y

    def move(self, delta):
        self.pos = self.pos + delta

    def set_name(self, name):
        self.name = name

    def set_skill(self, skill):
        pass

    def set_quest(self, quest):
        pass


class Hero(Actor):
    def set_skill(self, skill):
        self.skill = skill


class Enemy(Actor):
    def set_skill(self, skill):
        self.skill = skill


class NPC(Actor):
    def set_quest(self, quest):
        self.quest = quest


class GameFramework:
    def __init__(self):
        self.pygame = pygame
        self.screen = 0

        self.nY = 0
        self.nX = 0

        self.hero = 0
        self.monster = 0
        self.npc = 0

        print("init")

    def set_display(self, nX, nY):  # nX, nY = 스크린의 차원
        self.nY = nY
        self.nX = nX
        self.screen = self.pygame.display.set_mode(
            [self.nX, self.nY]
        )  # 스크린의 크기를 리스트 형태로 인자로 넘겨줌
        self.pygame.display.set_caption("Prince")  # 게임창의 이름

    def set_hero(self, hero: Actor):
        self.hero = hero

    def set_monster(self, monster: Enemy):
        self.monster = monster

    def set_npc(self, npc: NPC):
        self.npc = npc

    def ready(self):
        self.pygame.init()  # pygame 초기화

    def draw_polygon(self, color, points, thickness):
        self.pygame.draw.polygon(self.screen, color, points, thickness)

    def draw_edges(self):
        p1 = mv.MyVector(0, 0)
        p2 = mv.MyVector(0, 10)
        p3 = mv.MyVector(10, 0)

        self.draw_polygon(rgb["WHITE"], [p1.vec(), p2.vec(), p3.vec()], 1)

    def print_text(self, msg, color, pos):
        font = self.pygame.font.SysFont("consolas", 20)
        text_surface = font.render(msg, True, color, None)  # self.pygame.Color(color)
        text_rect = text_surface.get_rect()
        text_rect.topleft = pos
        self.screen.blit(text_surface, text_rect)

    # 게임 실행
    def launch(self):
        pass


class WhiteGame(GameFramework):
    def launch(self):
        print("launch")
        clock = self.pygame.time.Clock()

        delta = mv.MyVector(0, 0)

        key_flag = None

        done = False
        while not done:
            clock.tick(60)  # set on 30 frames per second

            for event in self.pygame.event.get():
                if event.type == self.pygame.QUIT:  # alt + f4
                    print("종료")
                    done = True

                elif event.type == self.pygame.KEYDOWN:  # 키를 눌렀을때
                    print("key down")
                    if event.key == self.pygame.K_LEFT:  # 어떤키가 눌렸는가?
                        print("K_LEFT")
                        delta.x = -5
                    elif event.key == self.pygame.K_RIGHT:
                        print("K_RIGHT")
                        delta.x = 5
                    elif event.key == self.pygame.K_DOWN:
                        print("K_DOWN")
                        delta.y = 5
                    elif event.key == self.pygame.K_UP:
                        print("K_UP")
                        delta.y = -5

                    key_flag = True

                elif event.type == self.pygame.KEYUP:
                    delta.setPos(0, 0)
                    print("key up")
                    key_flag = False

            if key_flag:
                self.hero.move(delta)  # 주인공의 위치가 업데이트가 됨

                print("pressed", self.hero.pos.getState())  # in console
                self.screen.fill(rgb["WHITE"])  # 특성을 살린 부분

                self.print_text(self.hero.name, rgb["RED"], self.hero.pos.vec())
                self.print_text(
                    self.hero.skill,
                    rgb["GREEN"],
                    (self.hero.pos + mv.MyVector(0, 15)).vec(),
                )

                self.print_text(self.npc.name, rgb["RED"], self.npc.pos.vec())
                self.print_text(
                    self.npc.quest,
                    rgb["GREEN"],
                    (self.npc.pos + mv.MyVector(0, 15)).vec(),
                )

                self.print_text(self.monster.name, rgb["RED"], self.monster.pos.vec())
                self.print_text(
                    self.monster.skill,
                    rgb["GREEN"],
                    (self.monster.pos + mv.MyVector(0, 15)).vec(),
                )

            self.pygame.display.flip()

        self.pygame.quit()


class BlackGame(GameFramework):
    def launch(self):
        print("launch")
        clock = self.pygame.time.Clock()

        delta = mv.MyVector(0, 0)

        key_flag = None

        done = False
        while not done:
            clock.tick(60)  # set on 30 frames per second

            for event in self.pygame.event.get():
                if event.type == self.pygame.QUIT:  # alt + f4
                    print("종료")
                    done = True

                elif event.type == self.pygame.KEYDOWN:  # 키를 눌렀을때
                    print("key down")
                    if event.key == self.pygame.K_LEFT:  # 어떤키가 눌렸는가?
                        print("K_LEFT")
                        delta.x = -5
                    elif event.key == self.pygame.K_RIGHT:
                        print("K_RIGHT")
                        delta.x = 5
                    elif event.key == self.pygame.K_DOWN:
                        print("K_DOWN")
                        delta.y = 5
                    elif event.key == self.pygame.K_UP:
                        print("K_UP")
                        delta.y = -5

                    key_flag = True

                elif event.type == self.pygame.KEYUP:
                    delta.setPos(0, 0)
                    print("key up")
                    key_flag = False

            if key_flag:
                self.hero.move(delta)  # 주인공의 위치가 업데이트가 됨

                print("pressed", self.hero.pos.getState())  # in console
                self.screen.fill(rgb["BLACK"])  # 특성을 살린 부분

                self.print_text(self.hero.name, rgb["RED"], self.hero.pos.vec())
                self.print_text(
                    self.hero.skill,
                    rgb["GREEN"],
                    (self.hero.pos + mv.MyVector(0, 15)).vec(),
                )

                self.print_text(self.npc.name, rgb["RED"], self.npc.pos.vec())
                self.print_text(
                    self.npc.quest,
                    rgb["GREEN"],
                    (self.npc.pos + mv.MyVector(0, 15)).vec(),
                )

                self.print_text(self.monster.name, rgb["RED"], self.monster.pos.vec())
                self.print_text(
                    self.monster.skill,
                    rgb["GREEN"],
                    (self.monster.pos + mv.MyVector(0, 15)).vec(),
                )

            self.pygame.display.flip()

        self.pygame.quit()


class YellowGame(GameFramework):
    def launch(self):
        print("launch")
        clock = self.pygame.time.Clock()

        delta = mv.MyVector(0, 0)

        key_flag = None

        done = False
        while not done:
            clock.tick(60)  # set on 30 frames per second

            for event in self.pygame.event.get():
                if event.type == self.pygame.QUIT:  # alt + f4
                    print("종료")
                    done = True

                elif event.type == self.pygame.KEYDOWN:  # 키를 눌렀을때
                    print("key down")
                    if event.key == self.pygame.K_LEFT:  # 어떤키가 눌렸는가?
                        print("K_LEFT")
                        delta.x = -5
                    elif event.key == self.pygame.K_RIGHT:
                        print("K_RIGHT")
                        delta.x = 5
                    elif event.key == self.pygame.K_DOWN:
                        print("K_DOWN")
                        delta.y = 5
                    elif event.key == self.pygame.K_UP:
                        print("K_UP")
                        delta.y = -5

                    key_flag = True

                elif event.type == self.pygame.KEYUP:
                    delta.setPos(0, 0)
                    print("key up")
                    key_flag = False

            if key_flag:
                self.hero.move(delta)  # 주인공의 위치가 업데이트가 됨

                print("pressed", self.hero.pos.getState())  # in console
                self.screen.fill(rgb["YELLOW"])  # 특성을 살린 부분

                self.print_text(self.hero.name, rgb["RED"], self.hero.pos.vec())
                self.print_text(
                    self.hero.skill,
                    rgb["GREEN"],
                    (self.hero.pos + mv.MyVector(0, 15)).vec(),
                )

                self.print_text(self.npc.name, rgb["RED"], self.npc.pos.vec())
                self.print_text(
                    self.npc.quest,
                    rgb["GREEN"],
                    (self.npc.pos + mv.MyVector(0, 15)).vec(),
                )

                self.print_text(self.monster.name, rgb["RED"], self.monster.pos.vec())
                self.print_text(
                    self.monster.skill,
                    rgb["GREEN"],
                    (self.monster.pos + mv.MyVector(0, 15)).vec(),
                )

            self.pygame.display.flip()

        self.pygame.quit()


# TODO
class GameFacade:
    def __init__(self, nX, nY):
        self.framework = GameFramework()
        self.framework.ready()
        self.framework.set_display(nX, nY)
        self.framework.draw_edges()

    def create_hero(self, x, y, name, skill):
        hero = Hero(x, y)
        hero.set_name(name)
        hero.set_skill(skill)
        self.framework.set_hero(hero)

    def create_enemy(self, x, y, name, skill):
        enemy = Enemy(x, y)
        enemy.set_name(name)
        enemy.set_skill(skill)
        self.framework.set_monster(enemy)

    def create_npc(self, x, y, name, quest):
        npc = NPC(x, y)
        npc.set_name(name)
        npc.set_quest(quest)
        self.framework.set_npc(npc)

    def launch_black_game(self):
        black_game = BlackGame()
        black_game.framework = self.framework
        black_game.launch()

    def launch_white_game(self):
        white_game = WhiteGame()
        white_game.framework = self.framework
        white_game.launch()

    def launch_yellow_game(self):
        yellow_game = YellowGame()
        yellow_game.framework = self.framework
        yellow_game.launch()


# game = BlackGame()
# game = WhiteGame()
game = YellowGame()
game.ready()
game.set_display(1500, 1000)
game.draw_edges()

hero = Hero(0, 0)
hero.set_name("prince")
hero.set_skill("swing a sword")

monster = Enemy(900, 450)
monster.set_name("JO ISU")
monster.set_skill("202103316")

npc = NPC(80, 80)
npc.set_name("village resident")
npc.set_quest("kill 100 monsters")

game.set_hero(hero)
game.set_monster(monster)
game.set_npc(npc)

game.launch()
