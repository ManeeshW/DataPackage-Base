function plot_3x1_1(x1, y1, ttl)

figure;

for i = 1:3
    subplot(3, 1, i)
    plot(x1, y1(i,:), 'LineWidth', 1)
    set(gca, 'FontName', 'Times New Roman', 'FontSize', 12);
end

suptitle(ttl);

end