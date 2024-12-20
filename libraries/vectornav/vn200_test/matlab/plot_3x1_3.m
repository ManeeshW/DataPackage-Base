function plot_3x1_3(x1, y1, x2, y2, x3, y3, ttl)

figure;

for i = 1:3
    subplot(3, 1, i)
    plot(x1, y1(i,:), 'LineWidth', 1)
    hold on;
    plot(x2, y2(i,:), 'LineWidth', 1);
    plot(x3, y3(i,:), 'LineWidth', 1);
    set(gca, 'FontName', 'Times New Roman', 'FontSize', 12);
    hold off;

end

suptitle(ttl, 'interpreter', 'latex');

end